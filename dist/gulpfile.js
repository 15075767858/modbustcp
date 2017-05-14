var gulp = require('gulp');
const changed = require('gulp-changed');
const ngAnnotate = require('gulp-ng-annotate'); // Just as an example

const SRC = '**/*.js';
const DEST = 'dist';

gulp.task('default', () =>
	gulp.src(SRC)
		.pipe(changed(DEST))
		// `ngAnnotate` will only get the files that
		// changed since the last time it was run
		.pipe(ngAnnotate())
		.pipe(gulp.dest(DEST))
);