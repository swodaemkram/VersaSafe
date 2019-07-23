<?php

/*
	Author: Gary Conway
	ajax_server.php
	this module handles AJAX requests for data
	it uses POST exclusively
	$_POST["action"] = stack, verify,reset or info
	$_POST{"which"] = "LEFT","RIGHT","ALL"

	CAUTION: any debug code that outputs anything to the screen, will totally kill
	any JSON packet return.

	This module is designed to be called exclusively via AJAX, and, in fact, will not respond otherwise

*/

    GLOBAL $socket, $api_connected, $port;
    GLOBAL $xml,$cfg, $host;

//NOTE: using the file option causes the client side to error out on the return data
$USEFILE=0;
$DEBUG=0;


//print "HELLO";

$port=1132;

$host="127.0.0.1";

$cfg=array
(
    "api_port"=>1132
);


if ($USEFILE)
{
	$myfile = fopen("/var/www/html/log.txt", "a+") ;
	fwrite($myfile,"----------------------NEW\n");
	fwrite($myfile,$_POST["action"]."\n");
	fwrite($myfile,$_POST["which"]."\n");
}

if (is_ajax()  || $DEBUG)
{

	if ($USEFILE)
		fwrite($myfile,"is ajax\n");

	// action
 	if (isset($_GET["action"]) && !empty($_GET["action"]))
		$action = strtolower($_GET["action"]);

	if (isset($_POST["action"]) && !empty($_POST["action"]))
		$action = strtolower($_POST["action"]);

define("START_STATE",0);
define ("RESULTS_STATE",1);
define ("IDLE_STATE",2);


	// state
	if (isset($_GET['state']) && !empty($_GET['state']) )
		$state = $_GET['state'];

    if (isset($_POST['state']) && !empty($_POST['state']) )
        $state = $_POST['state'];



	if ($action)
	{ //Checks if action value exists

		if (isset($_GET['which']) && $DEBUG) $which = strtoupper($_GET['which']);
    	if (isset($_POST['which']) && !$DEBUG) $which = strtoupper($_POST['which']);

//927-VALIDATOR-GET-RESULTS(LEFT|RIGHT)
//998-VALIDATOR_DONE(LEFT|RIGHT)      // used for VERIFY and STACK

//echo "<br>".$action;
    	switch($action)
	    { //Switch case for value of action
			case "config":
				$cmd="999-GET-CONFIG";
				call_API($cmd);
				break;
			case "utd_info":
				$cmd="907-UTD-INFO";
				call_API($cmd);
				break;
			case "utd_reset":
				$cmd="904-UTD-RESET";
                call_API($cmd);
				break;
			case "utd_inventory":
				// returns a string "0,0,0,0,0,0,0,0"
				$cmd="905-UTD-INVENTORY";
				call_API($cmd);
				break;

			case "stack":


				switch($state)
				{
				case START_STATE:
					$cmd="921-VALIDATOR-STACK-".$which;
					call_API($cmd);
					break;
				case RESULTS_STATE:
	                $cmd="927-VALIDATOR-GET-RESULTS-".$which;
					call_API($cmd);
					break;
				case IDLE_STATE:
					$cmd="926-VALIDATOR-IDLE-".$which;
					call_API($cmd);
					break;
				}

				break;

			case "verify":
            /*
                for verify, we turn on all connected/enabled validators and users may insert
                bills into any of them, returned results come from whichever device was used
                all this is handled at the driver level.
            */

                switch($state)
                {
                case START_STATE:
                    $cmd="920-VALIDATOR-VERIFY";
                    call_API($cmd);
                    break;
                case RESULTS_STATE:
                    $cmd="927-VALIDATOR-GET-RESULTS-".$which;
                    call_API($cmd);
                    break;
                case IDLE_STATE:
                    $cmd="926-VALIDATOR-IDLE-LEFT";
                    call_API($cmd);
                    $cmd="926-VALIDATOR-IDLE-RIGHT";
                    call_API($cmd);

                    break;
                }

                break;



//			case "info":
//				$cmd="927-VALIDATOR-GET-RESULTS-".$which;
//				call_API($cmd);
//				break;
			case "done":
				$cmd="998-VALIDATOR-DONE-".$which;		// stop result polling in driver
                call_API($cmd);
				break;
//	        case "stack":
//				$cmd="926-VALIDATOR-IDLE-".$which;
//				call_API($cmd);
//				break;
			case "info":
				$cmd="924-VALIDATOR-INFO-".$which;
                call_API($cmd);
				break;
//			case "verify":
//				$cmd="920-VALIDATOR-VERIFY";
//                call_API($cmd);
//				break;
			case "reset":
				$cmd="925-VALIDATOR-RESET-".$which;
                call_API($cmd);
				break;
	    }

  	}

}


if ($USEFILE)
	fclose($myfile);


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

//	$return = $_POST;

	// TESTS FOR DEBUGGING

//	$return["json"]= "USD:1000";					// stack/verify
//	$return["json"]= "NODATA";
//	$return["json"]="1,2,3,4,5,6,7,8";				// UTD INVENTORY
//	$return["json"]= "MODEL- JQN609:SERIAL- 1234:";	// MEI INFO
//	$return["json"]="UTD DRIVER - v1.01:UTD FIRMWARE - v2.05";



    SocketConnect();
    $res=SendMessage($cmd);
    $return=ReadMessage();
    $res_arr = explode(":",$return);
    $denom= $res_arr[0];
    $amt = (int) $res_arr[1]/100;

    $return["json"] =  $denom .":". $amt;
    CloseConnection();

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


