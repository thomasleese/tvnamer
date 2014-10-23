import sys


def main():
    try:
        if sys.stdin.isatty():
            raise ImportError("Running in a terminal.")
        from . import gui
        gui.main()
    except ImportError:
        from . import cli
        cli.main()
