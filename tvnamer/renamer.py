import os

import pytvdbapi.api as tvdb


class Renamer:
    def __init__(self, api_key):
        self.tvdb = tvdb.TVDB(api_key)

    def rename(self, show, season, directory):
        pass
