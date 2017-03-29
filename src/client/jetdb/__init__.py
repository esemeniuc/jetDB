from prompt_toolkit import prompt
from prompt_toolkit.completion import Completer, Completion
from prompt_toolkit.validation import Validator, ValidationError
from lrparsing import Grammar, Token, TokenRegistry, THIS, repr_parse_tree, Prio, repr_grammar, ParseError, TokenError


class CommandGrammar(Grammar):
    class T(TokenRegistry):
        name = Token(re=r'\w+')
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

class CommandCompleter(Completer):
    def find_completions(self, pt, cursor_pos, tp=None):
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
        categories = {
            'command': ['aaaac', 'bbbbc', 'ccc'],
            'param': ['aaaap', 'bbbbp', 'ccp'],
            'value': ['aaaav', 'bbbbv', 'ccv'],
        }
        cursor_pos = document.cursor_position
        pt = CommandGrammar.parse(document.text, on_error=handle_parse_error)
        t = self.find_completions(pt, cursor_pos)
        if t is not None:
            word, tp, pos = t
            return [Completion(w, start_position=pos-cursor_pos) for w in categories[tp] if w.startswith(word)]
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


def client():
    text = prompt('Give me some input: ', completer=CommandCompleter(), validator=CommandValidator())
    pt = CommandGrammar.parse(text)
    print('You said:', repr_parse_tree(pt))
