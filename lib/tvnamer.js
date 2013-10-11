var async = require("async");
var fs = require("fs");
var imdb = require("imdb-api");
var path = require("path");

var convertEpisodes = function(episodes) {
	var seasons = episodes.map(function(elem) { return parseInt(elem.season); })
				.sort(function(a, b) { return a - b; });
	seasons = seasons.filter(function(elem, pos) { return seasons.indexOf(elem) === pos }); // removes duplicates

	var result = [];
	for (var i = 0; i < seasons.length; i++) {
		var eps = episodes.filter(function(elem) { return elem.season === seasons[i]; });
		eps = eps.sort(function(a, b) { return a.number - b.number });
		eps = eps.map(function(elem) { return elem.name; });
		result.push(eps);
	}
	
	return result;
};

var getEpisodes = function(name, callback) {
	imdb.get(name, function(err, show) {
		if (err) return callback(err, null);

		show.episodes(function(err, episodes) {
			if (err) return callback(err, null);
			
			callback(null, convertEpisodes(episodes));
		});
	});
};

var padNumber = function(num) {
	if (num < 10) {
		return "0" + num;
	} else {
		return num.toString();
	}
};

var matchPaths = function(basePath, seasons) {
	var result = { };
	
	var seasonPaths = fs.readdirSync(basePath).sort();
	for (var i = 0; i < seasons.length; i++) {
		var season = seasons[i];
		if (season && seasonPaths[i]) {
			console.log("Matching season", i, "with", seasonPaths[i]);
			var seasonPath = path.join(basePath, seasonPaths[i]);
			var episodesPath = fs.readdirSync(seasonPath).sort();
			
			if (episodesPath.length === season.length) {
				for (var j = 0; j < episodesPath.length; j++) {
					var oldPath = path.join(seasonPath, episodesPath[j]);
					var newPath = path.join(seasonPath, padNumber(j + 1) + " - " + season[j].replace("/", "_") + path.extname(episodesPath[j]));
					if (oldPath !== newPath) {
						result[oldPath] = newPath;
					}
				}
			} else {
				console.log("Mismatch in numbers of episodes for", seasonPaths[i] + ". Expected", season.length, "but got", episodesPath.length, "episodes.");
			}
		}
	}
	
	return result;
};

module.exports = function(basepath, callback) {
	var showName = path.basename(basepath);
	getEpisodes(showName, function(err, episodes) {
		if (err) return callback(err, null);
		
		callback(null, matchPaths(basepath, episodes));
	});
};
