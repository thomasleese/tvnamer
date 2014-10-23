import re
import os

import pytvdbapi.api as tvdb


class Renamer:
    def __init__(self, api_key):
        self.tvdb = tvdb.TVDB(api_key)

    @staticmethod
    def flat_file_list(directory):
        directory = os.path.normpath(directory)

        for dirpath, dirnames, filenames in os.walk(directory):
            for filename in filenames:
                full_path = os.path.join(dirpath, filename)
                # remove directory from the start of the full path
                full_path = full_path[len(directory)+1:]
                yield full_path

    @staticmethod
    def normalise_params(params):
        def normalise(key, value):
            if key == "show":
                return str(value)
            elif key in ["episode", "season"]:
                return int(value)
            else:
                raise ValueError("Unknown parameter: '{}'.".format(key))

        return {key: normalise(key, value) for key, value in params.items()}

    @staticmethod
    def fill_out_params(params, tvdb):
        if "show" not in params or "season" not in params \
            or "episode" not in params:
            raise ValueError("'show', 'season', 'episode' must be provided.")

        show = tvdb.search(params["show"], "en")
        if len(show) != 1:
            raise ValueError("Ambigious show name.")
        show = show[0]
        season = show[params["season"]]
        episode = season[params["episode"]]

        if "episode_name" not in params:
            params["episode_name"] = episode.EpisodeName

        return params

    def rename_table(self, directory, input_regex, output_format):
        input_pattern = re.compile(input_regex)

        filenames = self.flat_file_list(directory)
        for filename in filenames:
            thing = input_pattern.search(filename)
            if thing is not None:
                params = self.normalise_params(thing.groupdict())
                params = self.fill_out_params(params, self.tvdb)
                output_filename = output_format.format(**params)
                yield filename, output_filename
