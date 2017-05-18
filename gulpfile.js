/*
var gulp = require('gulp');

const changed = require('gulp-changed');

var dest = "/Volumes/XiaoMi-usb0/共享/modbustcp/"

gulp.task('default', function () {
    // 将你的默认的任务代码放在这
    var watcher = gulp.watch('*', []);
    watcher.on('change', function (event) {
            console.log('File ' + event.path + ' was ' + event.type + ', running tasks...');
            var filename = event.path.substring(event.path.lastIndexOf("/")+1,event.path.length);
            var filepath = event.path.substring(process.cwd().length + 1, event.path.length)
            
            var destpath = dest+filepath.substring(0,filepath.lastIndexOf("/"));
            console.log(filename,destpath,filepath,dest);
            gulp.src(event.path).pipe(gulp.dest(destpath));
            //.pipe(gulp.dest(filepath))
    });
});
*/

var dest="/Applications/XAMPP/xamppfiles/htdocs/modbustcp"
var sources="/Volumes/XiaoMi-usb0/共享/modbustcp"

var gulp = require('gulp'),
    fileSync = require('gulp-file-sync');
gulp.task('file', function() {
  gulp.watch([dest+'/**/**'], function() {
    fileSync(dest,sources, {recursive: true,ignore: ["gulpfile.js",".DS_Store","._.DS_Store",".git","modbus-tcp-server"]});
  });
});
