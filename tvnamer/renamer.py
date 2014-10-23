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
            if key in ["series_name", "episode_name"]:
                return str(value)
            elif key in ["episode_number", "season_number"]:
                return int(value)
            else:
                raise ValueError("Unknown parameter: '{}'.".format(key))

        return {key: normalise(key, value) for key, value in params.items() if value is not None}

    @staticmethod
    def fill_out_params(params, tvdb):
        if "series_name" not in params or "season_number" not in params \
            or "episode_number" not in params:
            raise ValueError("'series_name', 'season_number', 'episode_number' must be provided.")

        search_results = tvdb.search(params["series_name"], "en")


        series = search_results[0]
        season = series[params["season_number"]]
        episode = season[params["episode_number"]]

        if "episode_name" not in params:
            params["episode_name"] = episode.EpisodeName

        return params

    def rename_table(self, directory, input_regex, output_format,
                     default_params=None):
        input_pattern = re.compile(input_regex)

        filenames = self.flat_file_list(directory)
        for filename in filenames:
            match = input_pattern.search(filename)
            if match is not None:
                params = dict(default_params or {})
                for key, value in match.groupdict().items():
                    params[key] = value

                params = self.normalise_params(params)
                params = self.fill_out_params(params, self.tvdb)
                output_filename = output_format.format(**params)
                yield filename, output_filename
