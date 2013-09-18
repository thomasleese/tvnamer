var async = require("async");
var fs = require("fs");
var imdb = require("imdb-api");
var path = require("path");

var convertEpisodes = function(episodes) {
	var seasons = episodes.map(function(elem) { return elem.season; }).sort();
	seasons = seasons.filter(function(elem, pos) { return seasons.indexOf(elem) === pos });
	
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

var convertPaths = function(basePath, episodes) {
	var result = { };
	
	var seasonPaths = fs.readdirSync(basePath).sort();
	for (var i = 0; i < episodes.length; i++) {
		if (seasonPaths[i]) {
			var seasonPath = path.join(basePath, seasonPaths[i]);
			var episodesPath = fs.readdirSync(seasonPath).sort();
			
			if (episodesPath.length == episodes[i].length) {
				for (var j = 0; j < episodes[i].length; j++) {
					var oldPath = path.join(seasonPath, episodesPath[j]);
					var newPath = path.join(seasonPath, padNumber(j + 1) + " - " + episodes[i][j] + path.extname(episodesPath[j]));
					result[oldPath] = newPath;
				}
			}
		}
	}
	
	return result;
};

module.exports = function(basepath, callback) {
	var showName = path.basename(basepath);
	getEpisodes(showName, function(err, episodes) {
		if (err) return callback(err, null);
		
		callback(null, convertPaths(basepath, episodes));
	});
};
