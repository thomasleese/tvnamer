#!/usr/bin/env python3
from setuptools import setup


with open("README.rst") as file:
    long_description = file.read()

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
        "pytvdbapi",
        "pyside"
    ],
    entry_points={
        "console_scripts": [
            "tvnamer = tvnamer:main",
            "tvnamer-cli = tvnamer.cli:main"
        ],
        'gui_scripts': [
            "tvnamer-gui = tvnamer.gui:main",
        ]
    },
    classifiers=[
        "Topic :: Internet",
        "Topic :: Multimedia :: Video",
        "Development Status :: 2 - Pre-Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.3",
        "Programming Language :: Python :: 3.4"
    ]
)
