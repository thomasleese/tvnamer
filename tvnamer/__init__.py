import sys


def main():
    try:
        if sys.stdin.isatty():
            # ImportError to avoid having to deal with two errors
            # instead can just be caught underneath
            raise ImportError("Running in a terminal.")
        from . import gui
        gui.main()
    except ImportError:
        from . import cli
        cli.main()
