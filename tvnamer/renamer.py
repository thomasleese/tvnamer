import re
import os

import pytvdbapi.api as tvdb


class Renamer:
    def __init__(self, api_key, directory, input_regex, output_format,
                 default_params=None):
        self.tvdb = tvdb.TVDB(api_key)
        self.directory = os.path.normpath(directory)
        self.input_pattern = re.compile(input_regex)
        self.output_format = output_format
        self.default_params = default_params or {}

    @property
    def flat_file_list(self):
        for dirpath, dirnames, filenames in os.walk(self.directory):
            for filename in filenames:
                full_path = os.path.join(dirpath, filename)
                # remove directory from the start of the full path
                full_path = full_path[len(self.directory)+1:]
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

        return {k: normalise(k, v) for k, v in params.items() if v is not None}

    @staticmethod
    def fill_out_params(params, tvdb):
        if "series_name" not in params or "season_number" not in params \
            or "episode_number" not in params:
            raise ValueError("'series_name', 'season_number', " \
                             "'episode_number' must be provided.")

        search_results = tvdb.search(params["series_name"], "en")

        series = search_results[0]
        season = series[params["season_number"]]
        episode = season[params["episode_number"]]

        if "episode_name" not in params:
            params["episode_name"] = episode.EpisodeName

        return params

    @property
    def table(self):
        for filename in self.flat_file_list:
            match = self.input_pattern.search(filename)
            if match is not None:
                params = dict(self.default_params)
                for key, value in match.groupdict().items():
                    params[key] = value

                params = self.normalise_params(params)
                params = self.fill_out_params(params, self.tvdb)
                output_filename = self.output_format.format(**params)
                yield filename, output_filename

    def perform_rename(self, table=None):
        if table is None:
            table = self.table

        for old, new in table:
            old_full_path = os.path.normpath(os.path.join(self.directory, old))
            new_full_path = os.path.normpath(os.path.join(self.directory, new))

            # ensure new directory exists
            new_dir = os.path.dirname(new_full_path)
            os.makedirs(new_dir, exist_ok=True)

            os.rename(old_full_path, new_full_path)
