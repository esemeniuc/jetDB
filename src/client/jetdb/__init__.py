from prompt_toolkit import prompt
from prompt_toolkit.completion import Completer, Completion
from prompt_toolkit.validation import Validator, ValidationError
from lrparsing import Grammar, Token, TokenRegistry, THIS, repr_parse_tree, Prio, repr_grammar, ParseError, TokenError
import zmq


class CommandGrammar(Grammar):
    class T(TokenRegistry):
        name = Token(re=r'[\w\.@]+')
        equals = Token('=')
    param = T.name + T.equals + T.name
    params = param + THIS | THIS*0
    command = T.name + params
    START = command

CommandGrammar.compile_grammar()

def handle_parse_error(iterator, input_tuple, stack):
    top = stack.pop()
    stack.append(top)
    if input_tuple[0] is CommandGrammar.T.equals:
        return []
    elif input_tuple[0] is CommandGrammar.T.name:
        return [
            (CommandGrammar.T.equals, '=', 0, 0, 0),
            (CommandGrammar.T.name, 'q', 0, 0, 0),
            input_tuple,
        ]
    elif len(input_tuple) == 1:
        if top[1][0][0] is CommandGrammar.T.equals:
            return [
                (CommandGrammar.T.name, 'q', 0, 0, 0),
                input_tuple,
            ]
        elif top[1][0][0] is CommandGrammar.T.name:
            return [
                (CommandGrammar.T.equals, '=', 0, 0, 0),
                (CommandGrammar.T.name, 'q', 0, 0, 0),
                input_tuple,
            ]

COMMANDS = {
    'login': {
        'name': None,
        'password': None,
    },
    'login2': {
        'email': None,
        'password': None,
    },
    'quit': {},
}

def map_ast(ast, fn):
    def map_start(ast):
        return fn((CommandGrammar.START, map_command(ast[1])))
    def map_command(ast):
        return fn((CommandGrammar.command, map_name(ast[1]), map_params(ast[2])))
    def map_name(ast):
        return fn(ast)
    def map_equals(ast):
        return fn(ast)
    def map_param(ast):
        return fn((CommandGrammar.param, map_name(ast[1]), map_equals(ast[2]), map_name(ast[3])))
    def map_params(ast):
        if len(ast) == 1:
            return fn(ast)
        else:
            return fn((CommandGrammar.params, map_param(ast[1]), map_params(ast[2])))
    return map_start(ast)

class CommandCompleter(Completer):
    def _find_completions(self, pt, cursor_pos, tp=None):
        if pt[0] is CommandGrammar.START:
            return self.find_completions(pt[1], cursor_pos)
        elif pt[0] is CommandGrammar.command:
            return self.find_completions(pt[1], cursor_pos, 'command') or self.find_completions(pt[2], cursor_pos)
        elif pt[0] is CommandGrammar.params:
            if len(pt) > 1:
                maybe = self.find_completions(pt[1], cursor_pos)
                if maybe is None:
                    return self.find_completions(pt[2], cursor_pos)
                return maybe
            else:
                return None
        elif pt[0] is CommandGrammar.param:
            return self.find_completions(pt[1], cursor_pos, 'param') or self.find_completions(pt[3], cursor_pos, 'value')
        elif pt[0] is CommandGrammar.T.name:
            if cursor_pos > pt[2] and cursor_pos <= pt[2]+len(pt[1]):
                return pt[1], tp, pt[2]
            else:
                return None
        else:
            return None

    def get_completions(self, document, complete_event):
        cursor_pos = document.cursor_position
        def find_completions(args):
            if args[0] is CommandGrammar.T.name:
                _, word, pos, _, _ = args
                if cursor_pos > pos and cursor_pos <= pos+len(word):
                    return True, word, pos
                else:
                    return False, word, pos
            elif args[0] is CommandGrammar.param:
                _, (is_name, name, name_pos), _, (is_value, value, value_pos) = args
                if is_name:
                    return 'param', name, name_pos
                elif is_value:
                    return 'value', value, value_pos, name
                else:
                    return None
            elif args[0] is CommandGrammar.command:
                _, (is_command, command, command_pos), other = args
                if is_command:
                    return 'command', command, command_pos
                elif other is not None:
                    return (*other, command)
                else:
                    return None
            elif args[0] is CommandGrammar.params:
                if len(args) != 1:
                    return args[1] or args[2]
                else:
                    return None
            else:
                return args[1]
        try:
            part = map_ast(CommandGrammar.parse(document.text, on_error=handle_parse_error), find_completions)
        except Exception:
            return []
        if part is not None:
            tp, word, pos = part[:3]
            if tp == 'command':
                completions = COMMANDS.keys()
            elif tp == 'param':
                command = part[3]
                completions = COMMANDS.get(command, {}).keys()
            elif tp == 'value':
                command = part[4]
                param = part[3]
                completer = COMMANDS.get(command, {}).get(param)
                if completer is not None:
                    completions = completer(value)
                else:
                    completions = []
            return [Completion(w, start_position=pos-cursor_pos) for w in completions if w.startswith(word)]
        else:
            return []

class CommandValidator(Validator):
    def validate(self, document):
        try:
            errpos = None
            def error_handler(iterator, input_tuple, stack):
                nonlocal errpos
                if len(input_tuple) > 2:
                    errpos = input_tuple[2]
                else:
                    p = stack.pop()
                    stack.append(p)
                    errpos = p[1][0][2]
            CommandGrammar.parse(document.text, on_error=error_handler)
        except ParseError as e:
            raise ValidationError(message=str(e), cursor_position=errpos) from e
        except TokenError as e:
            raise ValidationError(message=str(e), cursor_position=e.offset) from e

def command_tree_factory(*args):
    args = args[0]
    if args[0] is CommandGrammar.T.name:
        return args[1]
    elif args[0] is CommandGrammar.T.equals:
        return args[1]
    elif args[0] is CommandGrammar.param:
        return {args[1]: args[3]}
    elif args[0] is CommandGrammar.params:
        if len(args) == 1:
            return {}
        else:
            args[1].update(args[2])
            return args[1]
    elif args[0] is CommandGrammar.command:
        return {'operation': args[1], **args[2]}
    elif args[0] is CommandGrammar.START:
        return args[1]
    return None

def client():
    ctx = zmq.Context()
    sock = ctx.socket(zmq.REQ)
    sock.connect('tcp://localhost:5555')
    while True:
        text = prompt('> ', completer=CommandCompleter(), validator=CommandValidator())
        op = CommandGrammar.parse(text, tree_factory=command_tree_factory)
        if op['operation'] == 'quit':
            break
        sock.send_json(op)
        response = sock.recv_json()
        print(response)
