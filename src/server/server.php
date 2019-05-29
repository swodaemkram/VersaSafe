
<?php
$port=61000;
//$addr='127.0.0.1';
$addr='192.168.108.102';

print "Test PHP server II<br>\n";

set_time_limit(30);		// timeout timer

// create TCP stream socket
$sock = socket_create(AF_INET, SOCK_STREAM,0);
// bind the socket to the addr:port
socket_bind($sock, $addr, $port) or die ('Could not bind to address');
print "PHP Socket Server started at " . $addr . " on " . $port . "\n";


// and start listening
socket_listen($sock);

print "Listening on " . $addr. ":". $port ."\n";
while (true)
{
	$client = socket_accept($sock);
	$input = socket_read($client,1024);
	// echo back
	socket_write($client, $input);

	echo "Rcvd: ". $input . "\n";

	sleep(1);
}

socket_close($client);

socket_close($sock);


?>

