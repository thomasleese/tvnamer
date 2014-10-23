import os

import pytvdbapi.api as tvdb


class Renamer:
    include_extensions = [".avi"]

    def __init__(self, api_key):
        self.tvdb = tvdb.TVDB(api_key)

    @staticmethod
    def flat_file_list(directory, include_extensions=None):
        directory = os.path.normpath(directory)

        for dirpath, dirnames, filenames in os.walk(directory):
            for filename in filenames:
                full_path = os.path.join(dirpath, filename)
                # remove directory from the start of the full path
                full_path = full_path[len(directory)+1:]

                if include_extensions is not None:
                    if os.path.splitext(full_path)[1] not in include_extensions:
                        continue
                yield full_path

    def rename(self, directory):
        filenames = self.flat_file_list(directory, self.include_extensions)
        for file in filenames:
            print(file)
