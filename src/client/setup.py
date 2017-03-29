from setuptools import setup, find_packages

setup(
    name='jetdb',
    version='0.1',
    install_requires=['prompt_toolkit', 'lrparsing'],
    packages=['jetdb'],
    entry_points={
        'console_scripts': [
            'jetdb = jetdb:client',
        ]
    }
)
