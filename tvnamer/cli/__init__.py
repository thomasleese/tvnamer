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
    parser.add_argument("--series_name", type=str)
    parser.add_argument("--season_number", type=int)
    parser.add_argument("--episode_number", type=int)
    parser.add_argument("--episode_name", type=str)
    return parser


def main():
    parser = build_argument_parser()
    args = parser.parse_args()

    default_params = dict(series_name=args.series_name,
                          season_number=args.season_number,
                          episode_number=args.episode_number,
                          episode_name=args.episode_name)
    r = renamer.Renamer(args.api_key, args.directory, args.input_regex,
                        args.output_format, default_params)
    table = list(r.table)

    print("Assuming working directory of:", r.directory)
    for old, new in table:
        print(old, "->", new)

    input("Press enter to confirm.")

    r.perform_rename(table)
