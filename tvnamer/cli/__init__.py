import argparse

from tvnamer import renamer


def build_argument_parser():
    parser = argparse.ArgumentParser(description="Tool to rename TV files.")
    parser.add_argument("directory", help="Directory of TV files.")
    parser.add_argument("api_key", help="API key for TheTVDB.")
    parser.add_argument("input_regex",
                        help="Regular expression for the input file names.")
    parser.add_argument("output_format",
                        help="Output file name format string.")
    return parser

def main():
    parser = build_argument_parser()
    args = parser.parse_args()

    r = renamer.Renamer(args.api_key)
    table = r.rename_table(args.directory, args.input_regex,
                           args.output_format)
    for old, new in table:
        print(old, "->", new)
