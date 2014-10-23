import argparse

from tvnamer import renamer


def build_argument_parser():
    parser = argparse.ArgumentParser(description="Tool to rename TV files.")
    parser.add_argument("directory", help="Directory of TV files.")
    parser.add_argument("api_key", help="API key for TheTVDB.")
    return parser

def main():
    parser = build_argument_parser()
    args = parser.parse_args()

    r = renamer.Renamer(args.api_key)
    r.rename(args.directory)
