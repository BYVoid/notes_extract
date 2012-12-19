var initialize = function () {
  $.jRecorder({
    host: '/record',
    callback_started_recording: function () {
      callback_started();
    },
    callback_stopped_recording: function () {
      callback_stopped();
    },
    callback_activityLevel: function (level) {
      callback_activityLevel(level);
    },
    callback_activityTime: function (time) {
      callback_activityTime(time);
    },
    callback_finished_sending: function (time) {
      callback_finished_sending()
    },
    swf_path: '/javascripts/jRecorder.swf',
  });
  initialized = true;
};

var started = false;
$(function () {
  $('#record').click(function () {
    if (!started) {
      $.jRecorder.record(60);
    } else {
      $.jRecorder.stop();
    }
  });

  $('#send').click(function () {
    $.jRecorder.sendData();
  });
});

function callback_started() {
  started = true;
  $('#recorder').show();
  $('#status').html('Recording...');
  $('#record').html('Stop');
}

function callback_stopped() {
  started = false;
  $('#status').html('Finished.');
  $('#record').html('Record');
  $('#send').show();
}

function callback_error(code) {
  $('#status').html('Error, code:' + code);
}

function callback_finished_sending() {
  $('#status').html('Sending...');
  setTimeout(function () {
    window.location = '/latest';
  }, 3000);
}

function callback_activityLevel(level) {
  $('#level').html(level);
  if (level == -1) {
    $('#levelbar').css("width", "2px");
  } else {
    $('#levelbar').css("width", (level * 2) + "px");
  }
}

function callback_activityTime(time) {
  $('#time').html(time);
}

initialize();
