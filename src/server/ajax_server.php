<?php
/*
	Author: Gary Conway
	ajax_server.php
	this module handles AJAX requests for data
	it uses POST exclusively
	$_POST["action"] = stack, verify,reset or info
	$_POST{"which"] = "LEFT","RIGHT","ALL"

*/

    GLOBAL $socket, $api_connected, $port;
    GLOBAL $xml,$cfg, $host;

//print "HELLO";

$port=1132;

$host="127.0.0.1";

$cfg=array
(
    "api_port"=>1132
);


if (is_ajax())
{

  if (isset($_POST["action"]) && !empty($_POST["action"]))
	{ //Checks if action value exists
	    $action = $_POST["action"];
    	if (isset($_POST['which'])) $which = $_POST['which'];

    	switch($action)
	    { //Switch case for value of action
	        case "stack":
				$cmd="926-VALIDATOR-IDLE-".$which;
				call_API($cmd);
				break;
			case "info":
				$cmd="924-VALIDATOR-INFO-".$which;
                call_API($cmd);
				break;
			case "verify":
				$cmd="920-VALIDATOR-VERIFY";
                call_API($cmd);
				break;
			case "reset":
				$cmd="925-VALIDATOR-RESET-".$which;
                call_API($cmd);
				break;
	    }

  	}

}



//Function to check if the request is an AJAX request

function is_ajax()
{
	 return isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest';
}



/*
	call the API with a command and wait for the results
	return the results as a JSON packet

*/



function call_API($cmd)
{

//print"here";
//	$return = $_POST;

	$return["json"]= "USD:1000";
/*

    SocketConnect();
//    $cmd="926-VALIDATOR-IDLE-".$which;
    $res=SendMessage($cmd);
    $ret=ReadMessage();
    $res_ar = explode(":",$ret);
    $denom= $res_arr[0];
    $amt = (int) $res_arr[1]/100;

    $return["json"] =  $denom .":". $amt;
    CloseConnection();
*/
//	$return["json"]= json_encode($return);
	echo json_encode($return);

}



// returns true on success, else error message
function SocketConnect()
{
    GLOBAL $xml,$cfg,$socket,$port, $host, $api_connected;

    $api_connected=false;

//  $host="127.0.0.1";
//  $cfg["api_port"]=$xml->api->port;

//print "API_PORT:: ". $cfg['api_port'];

    set_time_limit(0);  // no time out


    $port = (int) $cfg["api_port"];

    // create a socket
    // returns a socket resource on success, else false on error
    $socket = socket_create(AF_INET, SOCK_STREAM, 0);

    if (! $socket)
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
        return $errormsg;
    }

    // connect to the server
    // returns true on success, else false
    $result = socket_connect($socket, $host, $port);

    if (! $result)
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
        return $errormsg;
    }

    $api_connected=true;
    return true;
}

function SendMessage($message)
{
    GLOBAL $socket, $port, $api_connected;

    if (!$api_connected) return;

    // write to server socket
    // returns number of bytes written, or false on error
    socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");

    // get the reply
    // returns a string on success, false on failure
    $result = socket_read ($socket, $port) or die("Could not read server response\n");

    if (! $result)
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
        return $errormsg;
    }

    echo "Reply From Server  :".$result ."</br>";

    if ($message == $result)
    {
        print "<br>SENT:RCVD messages match:: ". $result ."</br>";
    }

    return $result;
}



//    read a message from the API
//    RETURNS: a message from the API or an error message
//            returns "" is there is no data ready on the port


function ReadMessage()
{
    GLOBAL $socket, $port;
    // get the reply
    // returns a string on success, false on failure
    // returns "" when there is no data to read
    $result = socket_read ($socket, $port) or die("Could not read server response\n");

    if ($ret === "") return "";

    if ( $result === false)
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
        return $errormsg;
    }

    return $result;
}



function CloseConnection()
{
    GLOBAL $socket, $api_connected;

    if (! $api_connected) return;
    // tidy up
    socket_close($socket);

}


?>


