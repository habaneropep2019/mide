#!/bin/sh

echo "HTTP/1.0 200 OK
Content-type: text/html

<title>Mathopd test script</title>
<h1>Mathopd test script</h1>
<p>
Hello, world!</p>
<p>
I am <b>${SERVER_NAME}</b>,
running <b>${SERVER_SOFTWARE}</b>
on port <b>${SERVER_PORT}</b>.</p>
<p>
You must be <b>${REMOTE_HOST}</b>,
also known as <b>${REMOTE_ADDR}</b>.</p>"

for a in *
do
	if [ -d $a ]
	then
		echo "<p><a href=${a}/>${a}</a></p>"
	else
		echo "<p><a href=${a}>${a}</a></p>"
	fi
done
