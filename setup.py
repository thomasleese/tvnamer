#!/usr/bin/env python3
from setuptools import setup


with open("README.rst") as fd:
    long_description = fd.read()

setup(
    name="tvnamer",
    version="1.0.0-dev",
    description="Utility to rename lots of TV video files using the TheTVDB.",
    long_description=long_description,
    author="Tom Leese",
    author_email="tom@tomleese.me.uk",
    url="https://github.com/tomleese/tvnamer",
    packages=["tvnamer"],
    test_suite="tests",
    install_requires=[
        "pytvdbapi"
    ],
    entry_points = {
        "console_scripts": [
            "tvnamer = tvnamer:main",
            "tvnamer-cli = tvnamer.cli:main",
            "tvnamer-gui = tvnamer.gui:main",
        ]
    },
    classifiers=[
        "Topic :: Internet",
        "Topic :: Multimedia :: Video",
        "Development Status :: 1 - Planning",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.3",
        "Programming Language :: Python :: 3.4"
    ]
)
