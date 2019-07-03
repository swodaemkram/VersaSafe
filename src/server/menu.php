<?php

/*
    File: menu.php
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 5-25-2019
    Updated:


    This is a GUI designed to interact with the VSAFE API

	API interaction:

	some functions are directly implemented with immediate results in ajax_server.php, however, some
	are implemented as necessitated by the brain damaged MEI microcode as follows...

	STACK
	VERIFY
	INFO

	for the above cmds...
	1. this module calls the API directly and issues the appropriate command, EG 94-VALIDATOR-INFO-LEFT etc
	2. then the javascript timed event sends 927-VALIDATOR-GET-RESULTS-LEFT etc, to get the results of the above cmd
		if no results are ready, en empty string is returned from the API, otherwise, the data is returned


*/


session_start();    // start a user session

ini_set('html_errors',1);
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);



// for debugging only
$_SESSION['logged_in']='1'
//echo session_id();
//print_rx($_SESSION);
//print_rx($_POST);

?>

<html>
<head>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
	<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon" />
</head>

<?php


GLOBAL $conn,$xml,$cfg,$socket,$port, $host, $api_connected, $input_vars, $maxpacket;

$maxpacket=2000;


$input_vars = array();
require_once 'utils.inc';

$host="127.0.0.1";

$cfg=array
(
    "server"=>1,
    "user"=>2,
    "pw"=>3,
    "dbf"=>4,
    "dbf_port"=>5,
    "api_port"=>1132,			// set default
	"validator_timeout"=>30,	// default
);


// try to get the config info from the API, if that fails
// fall back to trying to read the local file
if (!GetConfig())
//ReadXML();
print_rx($xml);
exit();

get_input_vars();	// place all GET|POST vars in $input_vars array
//var_dump($input_vars);

if (extension_loaded('simplexml'))
	ConnectDBF();
else
	print "simplexml extension not loaded";




//var_dump($xml);

//https://dabuttonfactory.com/


//$menu="main";
//$function="";

if (isset($input_vars['m']))
	if ($input_vars['m'] == "")
		$menu="main";
	else
    	$menu = $input_vars['m'];
else
    $menu = "main";


if (isset($input_vars['f']))
	$function=$input_vars['f'];
else
	$function="";

//print_rx($input_vars);

print "MENU:: ". $menu. "  FUNCTION::". $function ."\n</br>"


?>

<!DOCTYPE html>
<html lang="en-US">
<head>
  <title>VSAFE API</title>
  <meta charset="utf-8">
  <link rel="stylesheet" href="menu.css">
</head>
<body>

<?php
print "<div class='topnav'>";
print "<a href='?m=main'>MAIN</a>";
if (CheckLoggedIn())
{
	print "<a href='?m=deposits'>DEPOSITS</a>";
	print "<a href='?m=withdrawl'>WITHDRAWL</a>";
	print "<a href='?m=reports'>REPORTS</a>";
	print "<a href='?m=settings'>SETTINGS</a>";
	print "<a href='?m=admin'>ADMIN</a>";
}
print "<span class='cpr'>Copyright 2019 FireKing Secutiry Group, All Rights Reserved</span>";
print "</div>";


$waiting=false;


function CheckLoggedIn()
{
    if (isset($_SESSION['logged_in']) AND  $_SESSION['logged_in']=='1')
			return TRUE;
	else	return FALSE;

}


/*
	get the configuration information via API

	RETURNS: TRUE on success
			else FALSE


*/
function GetConfig()
{
	GLOBAL $xml, $cfg;

    $cmd="999-GET-CONFIG";
    $ret=SocketConnect();

if ($ret != true)
    {
//print "<br><br> CONNECT FAILED1 <br><br>";
		return false;
 	}

	// returns false on error
	// else returns data from server
    $result=SendMessage($cmd);

print "RESULT::".$result;

	if ($result === false)
	{
		print "ERROR receiving data from server";
		$cfg["validator_timeout"]=30;
		CloseConnection();
		return false;
	}

    print $result;
    CloseConnection();

	$xml=simplexml_load_string($result);
	$cfg["validator_timeout"] = $xml->timeouts->validator;
	// access vars as
	// $xml->localDBF->ip;

	return true;
}





//=================================================
//					MENUS
//=================================================

switch($menu)
{
case "none":
	break;

default:
case "main":
    print "<div class='menu'>";
    print "<table class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "MAIN MENU";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";

    print "<tr>";

	if (CheckLoggedIn())
        print "<td><a  href='?m=logout'><img id='loginimg' src='img/button_logout.png' /> </a> </td>";
	else
	    print "<td><a  href='?m=login'><img id='loginimg' src='img/button_login.png' /> </a> </td>";

    print "<td><a href='?m=verify_bill'><img src='img/button_verify-bill.png' /></a> </td>";
    print "</tr>";

    print "</table>";
    print "</div>";
    break;


case "logout":
	$cmd="301-USER-LOGOUT";
//	$_SESSION['logged_in']='0';
    $ret=SocketConnect();
	$retstr=SendMessage($cmd);

    CloseConnection();
	session_destroy();
	header("Refresh:0;url=menu.php?m=main");
	break;

case "login":
    print "<div class='menu'>";

    print "<form id='loginformID' action='". htmlspecialchars($_SERVER["PHP_SELF"]) ."' method='post'>";
    print "<input type='hidden' id='fnlogin' name='m' value=''>";

    print "<table class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "USER LOGIN";
    print "</th>";
    print "</tr>";


	print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
	print "</tr";


    print "<tr>";
	print "<td class='user'>User Name</td>";
    print "<td><input class='user' id='uname' name='uname' type='text' maxlength='20' size0'20'></td>";
	print "</tr>";

	print "<tr>";
    print "<td class='user'>Password</td>";
    print "<td><input class='user' id='psw' name='psw' type='text' size='20' maxlength='20'></td>";
    print "</tr>";

    print "<tr>";
    print "<td>";
	print "<br><br>";
    print "</td>";
    print "</tr>";


    print "<tr>";
    print "<td><a href='?m=main'><img src='img/button_cancel.png' /> </a> </td>";
    print "<td><button class='unloadnow' id='dlogin' onclick='submitLOGIN(\"dologin\")'><img src='img/button_login.png'/></button></td>";
    print "</tr>";


    print "</table>";
	print "</div>";

	break;

case "dologin":


	// validate $input_vars['uname'] and $input_vars['psw']
	//300-USER-LOGIN-user-pw
	//301-USER-LOGOUT

	$cmd = "300-USER-LOGIN-". $input_vars['uname'] ."-". $input_vars['psw'];
	$ret=SocketConnect();
	if ($ret)
	{
		$_SESSION['logged_in']='0';
		$_SESSION['validated']='0';
		$_SESSION['username'] = $input_vars['uname'];
		$_SESSION['pw'] = $input_vars['psw'];

 		$retstr=SendMessage($cmd);
		if (strpos($retstr,"OK"))
		{
			$errormsg="User Validated";
			$_SESSION['logged_in']='1';
	        $_SESSION['validated']='1';


	}
		else
			$errormsg="Invalid User";
	}
	else
	{
		$errormsg=$ret;
	}

print $errormsg;
    CloseConnection();
    header("Refresh:0;url=menu.php?m=main");

	break;

case "verify":

	$errormsg=false;
    $ret=SocketConnect();	//returns TRUE on connection
	if ($ret ===TRUE)
	{
		$ret=SendMessage("920-VALIDATOR-VERIFY");
		if ($ret !=true)
		{
			$errormsg=$ret;
		}
	}
	else
	{
		$errormsg=$ret;
	}

	$waiting=true;

// the above must fall thru to the next case

case "verify_bill":
    print "<div class='menu'>";
    print "<table class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "VERIFY BILLS";
    print "</th>";
    print "</tr>";

	print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
	print "</tr>";

    print "<tr>";
    print "<td>";
	print "</td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?m=main'><img src='img/button_done.png' onclick='\"submitMEIDONE(\"998-VALIDATOR-DONE-'.which.'\"');'/> </a> </td>";
    print "<td><a href='?m=verify'><img src='img/button_verify.png' /> </a> </td>";
    print "</tr>";



    print "</table>";
	$start_time=time();

    if ($waiting)
    {
        print "<div class='results'>";
        print "results";
		if (!$errormsg)
		{
			print $errrormsg;
		}
		else
		{
			SendMessage("925-VALIDATOR-GET-RESULT");
			while ($waiting AND (time() - $start_time < (int) cfg['validator_timeout'] ) )
			{
				// returns "" if no data ready, else string from API or error msg
				$ret=ReadMessage();
				if ($ret )
				{
					print $ret;
				}
			}
		}

        print "</div>";
    }


    print "</div>";

    CloseConnection();

	break;

case "deposits":
	print "<div class='menu'>";
	print "<table class='mtable'>";

	print "<tr>";
	print "<th colspan='2' style='font-size:20pt;'>";
	print "DEPOSITS";
	print "</th>";
	print "</tr>";

	print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
	print "</tr>";

	print "<tr>";
	print "<td><a href='?f=deposit_cash'><img src='img/button_deposit-cash.png' /> </a> </td>";
	print "<td><a href='?f=manual_deposit'><img src='img/button_manual-deposit.png' /></a> </td>";
	print "</tr>";

    print "<tr>";
    print "<td><a href='?f=unlock_validator_gate'><img src='img/button_unlock-validator-gate.png' /> </a></td>";
    print "<td><a href='?f=reset_cassettes'><img src='img/button_reset-cassettes.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=close_user'><img src='img/button_close-user.png' /> </a></td>";
    print "</tr>";

	print "</table>";
	print "</div>";
	break;
case "withdrawl":
    print "<div class='menu'>";

    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "WITHDRAWL";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=vend_change'><img src='img/button_vend-change.png' /> </a></td>";
    print "</tr>";

    print "</table>";

    print "</div>";
	break;

case "reports":
    print "<div class='menu'>";

    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "REPORTS";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=reprint_receipts'><img src='img/button_reprint-receipts.png' /> </a></td>";
    print "<td><a href='?f=user_totals'><img src='img/button_user-totals.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=day_totals'><img src='img/button_day-totals.png' /> </a></td>";
    print "<td><a href='?f=shift_totals'><img src='img/button_shift-totals.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=user_trans_hist'><img src='img/button_user-trans-hist.png' /> </a></td>";
    print "<td><a href='?f=vend_inventory&m=none'><img src='img/button_vend-inventory.png' /> </a></td>";
    print "</tr>";


    print "</table>";

    print "</div>";
    break;



case "settings":
    print "<div class='menu'>";

    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "SETTINGS";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=change_pw'><img src='img/button_change-pw.png' /> </a></td>";
    print "<td><a href='?f=change_sound'><img src='img/button_change-sound.png' /> </a></td>";
    print "</tr>";

    print "</table>";

    print "</div>";
	break;
case "admin":
    print "<div class='menu'>";

    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "ADMIN";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=perform_eod'><img src='img/button_perform-eod.png' /> </a></td>";
    print "<td><a href='?f=content_removal'><img src='img/button_content-removal.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?m=maint'><img src='img/button_maint.png' /> </a></td>";
    print "<td><a href='?f=manage_users'><img src='img/button_manage-users.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=chg_order'><img src='img/button_chg-order.png' /> </a></td>";
    print "<td><a href='?f=vend_load'><img src='img/button_vend-load.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=vend_unload&m=none'><img src='img/button_vend-unload.png' /> </a></td>";
    print "<td><a href='?f=vend_define'><img src='img/button_vend-define.png' /> </a></td>";
    print "</tr>";



    print "</table>";


    print "</div>";

	break;


case "maint":
    print "<div class='menu'>";
    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "MAINT";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";


    print "<tr>";
	if ($xml->devices->validator_left == "enabled" OR $xml->devices->validator_right == "enabled")
    	print "<td><a href='?m=mei'><img src='img/button_mei.png' /> </a></td>";

	if ($xml->devices->utd == "enabled")
    	print "<td><a href='?m=utd'><img src='img/button_utd.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
	if ($xml->devices->pelicano =="enabled")
    	print "<td><a href='?m=pelicano'><img src='img/button_pelicano.png' /> </a></td>";

	if ($xml->devices->gsr50 == "enabled")
    	print "<td><a href='?m=gsr50'><img src='img/button_gsr.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?m=lock'><img src='img/button_lock.png' /> </a></td>";

	if ($xml->devices->ucd1 == "enabled" OR $xml->devices->ucd2 == "enabled")
    	print "<td><a href='?m=ucd'><img src='img/button_ucd.png' /> </a></td>";
    print "</tr>";

    print "<tr>";

	if ($xml->devices->validator_ucd == "enabled")
	    print "<td><a href='?m=jcm'><img src='img/button_jcm.png' /> </a></td>";
    print "</tr>";



	print "</table>";
    print "</div>";


	break;


case "utd":
    print "<div class='menu' id='utdmaintformID'>";

//    print "<form id='utdmaintformID' action='". htmlspecialchars($_SERVER["PHP_SELF"]) ."' method='post'>";

    print "<input type='hidden' id='fnutd' name='f' value=''>";
    print "<input type='hidden' id='mUTD' name='m' value=''>";


    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "UTD MAINT";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";

/*
	UNLOAD COLUMN
	UNLOAD ALL
	LOAD
	RESET
*/

    if ($xml->devices->utd == "enabled")
	{
	    print "<tr>";
        print "<td><br><button class='buttons' onclick='submitUTDMAINT(\"utd_reset\",1,\"utd\");'><img src='img/button_reset.png' /> </button></td>";
        print "<td><br><button class='buttons' onclick='submitUTDMAINT(\"utd_inventory\",1,\"utd\");'><img src='img/button_inventory.png' /> </button></td>";
	    print "</tr>\n";

        print "<tr>";
        print "<td><button class='buttons' id='utd_info_btn' onclick='submitUTDMAINT(\"utd_info\",1,\"utd\");'><img src='img/button_info.png'/></button></td>";
	    print "<td><button class='buttons' id='utd_unloadall' onclick='submitUNLOAD(\"vend_unload_all\");'><img src='img/button_unload-all.png'/></button></td>";
        print "</tr>\n";

		print "<tr>";
        print "<td></td>";
        print "<td><button class='buttons' id='utd_unloadnow' onclick='submitUNLOAD(\"vend_unload_now\");'><img src='img/button_unload-now.png'/></button></td>";
        print "</tr>\n";


	}

    print "</table>";
//	print "</form>";
    print "</div>";


    break;



case "mei":
    print "<div class='menu' id='meimaintformID'>";

//    print "<form id='meimaintformID1' action='". htmlspecialchars($_SERVER["PHP_SELF"]) ."' method='post'>";

    print "<input type='hidden' id='fnmei' name='f' value=''>";
    print "<input type='hidden' id='mMEI' name='m' value=''>";


    print "<table  class='mtable'>";

    print "<tr>";
    print "<th colspan='2' style='font-size:20pt;'>";
    print "MEI MAINT";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th colspan='2'><hr class='hrr'></th>";
    print "</tr>";


    print "<tr>";
    if ($xml->devices->validator_left =="enabled")
	    print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_reset_left\",1,\"mei\");'><img src='img/button_reset-left.png' /> </button></td>";
    if ($xml->devices->validator_right == "enabled" )
        print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_reset_right\",1,\"mei\");'><img src='img/button_reset-right.png' /> </button></td>";
    print "</tr>\n";


    print "<tr>";
    if ($xml->devices->validator_left =="enabled")
	    print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_info_left\",1,\"mei\");'><img src='img/button_info-left.png' /></button></td>";
    if ($xml->devices->validator_right == "enabled" )
	    print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_info_right\",1,\"mei\");'><img src='img/button_info-right.png' /></button></td>";
    print "</tr>\n";

    print "<tr>";
    if ($xml->devices->validator_left =="enabled")
	    print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_stack_left\",1,\"mei\");'><img src='img/button_stack-left.png' /></button></td>";
    if ($xml->devices->validator_right == "enabled" )
	    print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_stack_right\",1,\"mei\");'><img src='img/button_stack-right.png' /></button></td>";
    print "</tr>\n";


    print "<tr>";
	print "<td><br><button class='buttons' onclick='submitMEIMAINT(\"mei_verify\",1,\"mei\");'><img src='img/button_verify.png' /> </button></td>";
    print "</tr>\n";


	print "</table>";
//	print "</form>";
    print "</div>";


    break;

}

$VALIDATOR_LEFT=0;
$VALIDATOR_RIGHT=1;
$VALIDATOR_UCD=2;


//========================================================
//					FUNCTIONS
//========================================================
switch($function)
{

case "utd_info":
    print "<div class='function'>";
    print "UTD INFO";
    print "<div class='data' id='utd_info'>test</div>";
    print "</div>";
    break;


case "utd_inventory":
    print "<div class='function'>";
    print "UTD INVENTORY";
    print "<div class='data' id='utd_inventory'>test</div>";
    print "</div>";
	break;

//925-VALIDATOR-RESET-(LEFT|RIGHT)
case "mei_reset_left":
    print "<div class='function'>";
    print "RESETTING LEFT VALIDATOR";
    print "<div class='data' id='mei_reset_left'></div>";
	MEI_reset($VALIDATOR_LEFT);
    print "</div>";
	break;

case "mei_reset_right":
    print "<div class='function'>";
    print "RESETTING RIGHT VALIDATOR";
    print "<div class='data' id='mei_reset_right'></div>";
    MEI_reset($VALIDATOR_RIGHT);
    print "</div>";
	break;

//920-VALIDATOR-VERIFY
//TODO - must make this an active display
case "mei_verify":
    print "<div class='function'>";
    print "VALIDATOR - VERIFY BILLS";
    print "<div class='data' id='mei_verify'></div>";
    $res=MEI_verify();
    print "</div>";
    break;


//921-VALIDATOR-STACK-(LEFT|RIGHT|UCD)
case "mei_stack_left":
    print "<div class='function'>";
    print "STACKING LEFT VALIDATOR";
    print "<div class='data' id='mei_stack_left'>test</div>";
    $res=MEI_stack($VALIDATOR_LEFT);
    print "</div>";
    break;
case "mei_stack_right":
    print "<div class='function'>";
    print "STACKING RIGHT VALIDATOR";
    print "<div class='data' id='mei_stack_right'></div>";
    $res=MEI_stack($VALIDATOR_RIGHT);
    print "</div>";
    break;

//924-VALIDATOR-INFO-(LEFT|RIGHT|UCD)
case "mei_info_left":
    print "<div class='function'>";
    print "LEFT VALIDATOR INFO";
	print "<div class='data' id='mei_info_left'></div>";
    MEI_info($VALIDATOR_LEFT);
    print "</div>";
    break;

case "mei_info_right":
    print "<div class='function'>";
    print "RIGHT VALIDATOR INFO";
    print "<div class='data' id='mei_info_right'></div>";
    MEI_info($VALIDATOR_RIGHT);
    print "</div>";
    break;


case "deposit_cash":
	print "<div class='function'>";
	print "DEPOSIT CASH";
	print "</div>";
	DepositCash();
	break;

case "unlock_validator_gate":
    print "<div class='function'>";
    print "</div>";
	UnlockValidator();
	break;
case "close_user":
    print "<div class='function'>";
    print "</div>";
	CloseUser();
	break;
case "manual_deposit":
    print "<div class='function'>";
    print "</div>";
	ManualDeposit();
	break;
case "reset_cassettes":
    print "<div class='function'>";
    print "</div>";
	ResetCassettes();
	break;

// widthdrawl
case"vend_change":
    print "<div class='function'>";
    print "</div>";
	VendChange();
	break;

//reports
case "reprint_receipts":
    print "<div class='function'>";
    print "</div>";
	ReprintReceipts();
	break;
case "user_totals":
    print "<div class='function'>";
    print "</div>";
	UserTotals();
    break;
case "day_totals":
    print "<div class='function'>";
    print "</div>";
	DayTotals();
    break;
case "shift_totals":
    print "<div class='function'>";
    print "</div>";
	ShiftTotals();
    break;
case "user_trans_hist":
    print "<div class='function'>";
    print "</div>";
	UserTransHist();
    break;
case "vend_inventory":
    print "<div class='function'>";
	VendInventory();
    print "</div>";
    break;



// settings
case "change_pw":
    print "<div class='function'>";
    print "</div>";
	ChangePW();
	break;
case "change_wsound":
    print "<div class='function'>";
    print "</div>";
	break;


//admin
case "manage_users":
    print "<div class='function'>";
    print "</div>";
	ManageUsers();
	break;

case "chg_order":
    print "<div class='function'>";
    print "</div>";
	ChgOrder();
	break;

case "perform_eod":
    print "<div class='function'>";
    print "</div>";
	PerformEOD();
	break;

case "content_removal":
    print "<div class='function'>";
    print "</div>";
	ContentRemoval();
	break;


case "vend_unload":
    print "<div class='unload'>";

	print "<form id='unloadformID' action='". htmlspecialchars($_SERVER["PHP_SELF"]) ."' method='post'>";

	print "<input type='hidden' id='fn' name='f' value=''>";
	print "<input type='hidden' id='m' name='m' value=''>";
	print "<table class='columns'>\n";


        print "<tr>\n";
        print "<th colspan='2'>VEND UNLOAD</th>";
        print "</tr>\n";

        print "<tr>";
		print "<th>COLUMN</th><th>COLUMN</th>";
        print "</tr>\n";

        print "<tr>";
		print "<th class='hrr' colspan='2'><hr></th>";
        print "</tr>\n";

        print "<tr>";
		print "<td></br></td>";
        print "</tr>\n";


	for ($n=1; $n <5; $n++)
	{
		print "<tr>\n";
		print "<td><input type='checkbox' name='col".$n ."' value='". $n ."'>".$n."</td>\n";
        print "<td><input type='checkbox' name='col". ($n+4) ."' value='". ($n+4) ."'>". ($n+4)."</td>\n";
	    print "</tr>\n";
	}

        print "<tr>";
        print "<td></br></td>";
        print "</tr>\n";


	print "<tr>\n";
	print "<td><button class='buttons' id='unloadnow' onclick='submitUNLOAD(\"vend_unload_now\");'><img src='img/button_unload-now.png'/></button></td>";
    print "<td><button class='buttons' id='unloadall' onclick='submitUNLOAD(\"vend_unload_all\");'><img src='img/button_unload-all.png'/></button></td>";
    print "</tr>\n";

    print "<tr>";
    print "<td><br><button class='buttons' onclick='submitUNLOADCANCEL(\"admin\");'><img src='img/button_cancel.png' /> </button></td>";
    print "</tr>\n";

	print "</form>\n";

	print "</table>\n";



    print "</div>";
	break;

case "vend_unload_all":
    print "<div class='function'>";
    print "</div>";
    VendUnloadAll();
    break;

// unload selected columns
case "vend_unload_now":
    print "<div class='function'>";

	if ($_SERVER["REQUEST_METHOD"] == "POST")
	{
print "WAS POST";

	$cols = Array(0,0,0,0,0,0,0,0,0);

	// active the selected columns in the array
	for ($n=1; $n <=8; $n++)
	{
		$mycol = "col".$n;
		if (isset($_POST[$mycol]))
			$cols[$n]=check_input($_POST[$mycol]);

	}

//	print_rx($cols);

//	print_rx ($_POST);


	} // end CHK POST
	else
	{
		print "WASNT POST";
	}


	$cmd="906-UTD-UNLOAD-COLS-";

	// append the selected colunns to the above command string
	for ($n=1; $n<=8; $n++)
	{
		if ($cols[$n] == $n AND $n!=8)
			$cmd = $cmd . $cols[$n] . ",";

		// handle last one with adding ","
		if ($cols[$n] == $n AND $n==8)
			$cmd .= $cols[$n];

	}



	print "<br>API CMD:". $cmd;

    SocketConnect();
    SendMessage($cmd);
    CloseConnection();


    print "</div>";
    break;


case "vend_load":
    print "<div class='function'>";
    print "</div>";
	VendLoad();
	break;

case "vend_define":
    print "<div class='function'>";
    print "</div>";
	VendDefine();
	break;
}


// MUST BE CALLED AFTER CANCELLING A DEPOSIT OR VERIFY OR A TIMEOUT FOR EITHER
//926-VALIDATOR-IDLE-(LEFT|RIGHT)
function MEI_idle($which)
{
    $cmd="926-VALIDATOR-IDLE-";

    switch($which)
    {
    case $VALIDATOR_LEFT:
        $cmd .="LEFT";
        break;
    case $VALIDATOR_RIGHT:
        $cmd .="RIGHT";
        break;
    case $VALIDATOR_UCD:
        $cmd .="UCD";
        break;
    }

    SocketConnect();
    $res=SendMessage($cmd);
    CloseConnection();

}



// need to make this an active display
function MEI_stack($which)
{
	GLOBAL $cfg;

    $cmd="921-VALIDATOR-STACK-";


    switch($which)
    {
    case $VALIDATOR_LEFT:
		$ext="LEFT";
        $cmd .="LEFT";
        break;
    case $VALIDATOR_RIGHT:
		$ext="RIGHT";
        $cmd .="RIGHT";
        break;
    case $VALIDATOR_UCD:
		$ext="UCD";
        $cmd .="UCD";
        break;
    }

    $start_time=time();

    SocketConnect();

	while (time() - $start_time < (int) cfg['validator_timeout'] )
	{
		// return is...
		// "USD:100" for a one dollar bill
	    $res=SendMessage($cmd);
		$ret=ReadMessage();
		if ($ret)
		{
			$res_ar = explode(":",$ret);
			$denom= $res_arr[0];
			$amt = (int) $res_arr[1]/100;

			print "Stacked {$amt}  {$denom}";
		}

	}


	$cmd="926-VALIDATOR-IDLE-".$ext;
    $res=SendMessage($cmd);

    CloseConnection();
}


// verify bills on any of the enabled validators
function MEI_verify()
{
    $cmd="920-VALIDATOR-VERIFY";
    SocketConnect();
    $res=SendMessage($cmd);
    CloseConnection();
	return $res;
}



function MEI_reset($which)
{
    $cmd="925-VALIDATOR-RESET-";

	switch($which)
	{
	case $VALIDATOR_LEFT:
		$cmd .="LEFT";
		break;
	case $VALIDATOR_RIGHT:
		$cmd .="RIGHT";
		break;
	case $VALIDATOR_UCD:
		$cmd .="UCD";
		break;
	}

    SocketConnect();
    SendMessage($cmd);
    CloseConnection();
}




//998-VALIDATOR_DONE(LEFT|RIGHT)      // used for VERIFY and STACK
function MEI_done($which)
{
   $cmd="998-VALIDATOR_DONE-";
    switch($which)
    {
    case $VALIDATOR_LEFT:
        $cmd .="LEFT";
        break;
    case $VALIDATOR_RIGHT:
        $cmd .="RIGHT";
        break;
    case $VALIDATOR_UCD:
        $cmd .="UCD";
        break;
    }
    SocketConnect();
    $res=SendMessage($cmd);
    CloseConnection();
    return $res;
}



//924-VALIDATOR-INFO-(LEFT|RIGHT|UCD)
function MEI_info($which)
{
	$cmd="924-VALIDATOR-INFO-";
    switch($which)
    {
    case $VALIDATOR_LEFT:
        $cmd .="LEFT";
        break;
    case $VALIDATOR_RIGHT:
        $cmd .="RIGHT";
        break;
    case $VALIDATOR_UCD:
        $cmd .="UCD";
        break;
    }

    SocketConnect();
    $result=SendMessage($cmd);
	print $result;
    CloseConnection();

	return $result;
}



function ReprintReceipts()
{
}

function DayTotals()
{
}

function UsrTotals()
{
}

function ShiftTotals()
{
}


function UserTransHist()
{
}


function VendInventory()
{
	GLOBAL $conn;

	$sql="SELECT a.col,a.tube_name,a.tube_value,a.tube_count, b.currency_code,b.type,b.symbol, (a.tube_count*a.tube_value)/100 AS dollars ";
	$sql .= "FROM utd_inventory AS a ";
	$sql .= "INNER JOIN currency AS b ON b.denom_code=a.denom_code";


	$result = mysqli_query($conn, $sql);


if ($result)
{
$numrows = mysqli_num_rows($result);

//print "NUM_ROWS: ". $numrows;

    if ($numrows >0)
    {
		print "<table class='invtable'>";
		print "<tr><thcolspan=3;>UTD INVENTORY</th></tr>";
		print "<tr><th>COL NAME</th><th>QTY</th><th>VALUE</th><th>CURRENCY</th></tr>";
        while ($row = mysqli_fetch_assoc($result))
		{
			print "<tr>";
			print "<td>".$row["tube_name"] ."</td><td>". $row["tube_count"]. "</td><td>". number_format($row["dollars"],2) ."<td>". $row["currency_code"] . "</td></br>";
			print "</tr>";
		}
		print "</table>";
	}

	mysqli_free_result($result);
}
else
	print "BAD RESULT";

}


function DepositCash()
{
	SocketConnect();
	SendMessage("LOCK-DOOR");
	CloseConnection();
}


function UnlockValidator()
{
}


function CloseUser()
{
}


function ManualDeposit()
{
}

function ResetCassettes()
{
}


function VendChange()
{
}


function ChangePW()
{
}


function ChangeSound()
{
}


function ManageUsers()
{
}


function ChgOrder()
{
}


function PerformEOD()
{
}


function ContentRemoval()
{
}

function Maint()
{
}


function VendLoad()
{
}


function VendUnload()
{
}


function VendUnloadAll()
{
    SocketConnect();
    SendMessage("900-UTD-UNLOADALL");
    CloseConnection();
}




function VendDefine()
{
}


function queryDBF($sql)
{
	GLOBAL $conn;

	$result = mysqli_query($conn,$sql);

	if (mysqli_num_rows($result) >0)
	{
		while ($row = mysqli_fetch_assoc($result))
		{
		}
	}

}


function ReadXML()
{
	GLOBAL $xml;

    // our config file
    $configpath="/home/garyc/Desktop/VersaSafe/git/VersaSafe/src/xml/config.xml";
    // read it into $xml obj
    $xml=simplexml_load_file($configpath);

//var_dump($xml);
}




function check_input($data)
{
	$data = trim($data);
	$data = stripslashes($data);
	$data = htmlspecialchars($data);
	return $data;
}




function ConnectDBF()
{
	GLOBAL $conn,$xml, $cfg;




	//  print_rx($xml);

//	echo $xml->localDBF->ip;
	$cfg["server"]=$xml->localDBF->ip;
	$cfg["user"]=$xml->localDBF->user;
	$cfg["pw"]=$xml->localDBF->password;
	$cfg["dbf"]=$xml->localDBF->database;
	$cfg["dbf_port"]=$xml->localDBF->port;

/*
	$dbf_server = $xml->localDBF->ip;
	$dbf_user= $xml->localDBF->user;
	$dbf_pw= $xml->localDBF->password;
	$dbf_dbf = $xml->localDBF->database;
	$dbf_port = $xml->localDBF->port;
*/

	// connect to mySQL
	$conn = new mysqli($cfg["server"],$cfg["user"],$cfg["pw"],$cfg["dbf"]);
	if (!$conn->connect_error)
	{
		print mysqli_connect_error();
	}

	$conn->set_charset('utf8');
}


function print_rx($arr)
{
    echo "<pre>"; print_r($arr); echo "</pre>";
}



//$conn->close();
mysqli_close($conn);



echo "</body>";
echo "</html>";


// NOTE: no close tag... is the new NORM in PHP 7
// zend has insisted on that for some time


// returns true on success, else error message
function SocketConnect()
{
	GLOBAL $xml,$cfg,$socket,$port, $host, $api_connected;

	$api_connected=false;


//	$host="127.0.0.1";
//	$cfg["api_port"]=$xml->api->port;

//print "API_PORT:: ". $cfg['api_port'];

	set_time_limit(0);	// no time out


	$port = (int) $cfg["api_port"];

//print $cfg["api_port"];

	// create a socket
    // returns a socket resource on success, else false on error
	$socket = socket_create(AF_INET, SOCK_STREAM, 0);

	if ( $socket)
	{
		print "Socket created<br>";
	}
	else
	{
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
        return $errormsg;
	}

	// connect to the server
	// returns true on success, else false
	$result = socket_connect($socket, $host, $port);

	if ($result)
	{
		print "Socket Connected<br>";
	}
	else
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);
		socket_clear_error();
print "SOCKETCONNECT:: ".$errormsg."<br>";
		return $errormsg;
	}

//	socket_set_nonblock($socket);
//	stream_set_timeout ( resource $stream , int $seconds [, int $microseconds = 0 ] ) : bool
//	socket_set_timeout($socket,30);
//	$status = sock_get_status($socket);

	$api_connected=true;
	return true;
}


/*
	sends msg and receives response

	RETURNS: received data
			-or- false on error

*/

function SendMessage($message)
{
	GLOBAL $socket, $port, $api_connected, $maxpacket;

	if (!$api_connected) return false;

	// write to server socket
	// returns number of bytes written, or false on error
	$sent =socket_write($socket, $message, strlen($message));

	if ($sent == false)
	{
		return false;
	}

	sleep(1);

	//    PHP_BINARY_READ (Default) - use the system recv() function. Safe for reading binary data.
	//    PHP_NORMAL_READ - reading stops at \n or \r.

	// get the reply
	// returns a string on success, false on failure
	$result = socket_read ($socket, $maxpacket,PHP_BINARY_READ);

/*
	if ($result == "")
	{
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
print "GCMSG:".$errormsg."<br>";
//        return $errormsg;
		return false;
	}
*/

	echo "Reply From Server  :".$result ."</br>";

/*
    if ($message == $result)
	{
		print "<br>SENT:RCVD messages match:: ". $result ."</br>";
	}
*/
	return $result;
}


/*
	read a message from the API
	RETURNS: a message from the API or an error message
			returns "" is there is no data ready on the port
*/

function ReadMessage()
{
	GLOBAL $socket, $maxpacket;


    // get the reply
    // returns a string on success, false on failure
	// returns "" when there is no data to read
    $result = socket_read ($socket, $maxpacket,PHP_BINARY_READ);

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

/*
function getvars()
{
    $postvars = array('testkey' => 'testval');

	foreach ($_POST as $k=>$v)
	{
		$postvars[$k] = $v;
	}
	$n=extract($postvars);
//print "numvars:: ". $n ."<br>";
	var_dump(get_defined_vars());

//print "m::". $m ."<br>";
//print "user::". $uname ."<br>";
//print "psw::". $psw ."<br>";


}
*/


?>

<script language="javascript" type="text/javascript">

	var timeouttimer;
    var which="ALL";
    var action;
	var prev_menu;
	var caller; //   will contain one of the following...
    	        //"mei_stack_left|right"
	            //"mei_info_left|right"
	            //"mei_reset_left|right"
	            //"mei_verify"

	var callTimer;

	var INTERVAL=150;       // trigger AJAX calls INTERVRAL ms apart
	var TIMEOUT=30000;		// ms, timeout for verifying or stacking bills
	var totalStacked=0; 	// must divide by 100 to display
	var denom;
	var strng;


$(document).ready(function()
{
    // call getStack every 100ms
//  window.setInterval(getStack,5000);
//  alert("doc is ready");
});



    function submitUNLOAD(fn)
	{
        alert(fn);
		$("#fn").val(fn);
//		e.preventDefault();
       $("#unloadformID").submit();
    }

    function submitUNLOADCANCEL(fn)
    {
        $("#m").val(fn);
       $("#unloadformID").submit();
    }



	function submitLOGIN(fn)
	{
		$("#fnlogin").val(fn);
		$("#loginformID").submit();
	}


function submitMEIDONE(fn)
{
	setActionWhich(fn);
	startAJAX(fn);

}


    function submitMEIMAINT(fn,AJAX,prev)
    {
//		alert(fn);
		setActionWhich(fn);
		prev_menu=prev;

		if (AJAX == 1)
			startAJAX(fn);
//        $("#mMEI").val("none");
//        $("#fnmei").val(fn);
//        $("#meimaintformID").submit();
		clearDIV("#meimaintformID",fn);

    }


	function submitUTDMAINT(fn,AJAX,prev)
	{
        setActionWhich(fn);
        prev_menu=prev;

        if (AJAX == 1)
            startAJAX(fn);

//		$("#fnutd").val(fn);
//		$("mUTD").val("none");
//		$("#utdmaintformID").submit();
//		caller = "utdmaintformID";
		clearDIV("#utdmaintformID",fn);

	}



/*
    print "<div class='function'>";
    print "STACKING LEFT VALIDATOR";
    print "<div class='data' id='mei_stack_left'></div>";
//    $res=MEI_stack($VALIDATOR_LEFT);
    print "</div>";

*/
function clearDIV(divID,fn)
{
//	alert("clearing " + divID + " "+ fn);
	var cmd;

	switch(action)
	{
	case "utd_reset":
		cmd = $(divID).html();	// just get the original string, we wont change it
		break;
	case "utd_info":
       cmd="<span class='hdr'>UTD INFO</span>";
        cmd +="<div id='doneID' class='done'>";
        cmd+="<a href='?m="+prev_menu+"'><img src='img/button_done.png'  > </a>";
        cmd+="</div>";
		break;
	case"utd_inventory":
        cmd="<span class='hdr'>UTD INVENTORY</span>";
        cmd +="<div id='doneID' class='done'>";
        cmd+="<a href='?m="+prev_menu+"'><img src='img/button_done.png'  > </a>";
        cmd+="</div>";
        break;

	case "stack":
		cmd="<span class='hdr'>STACKING LEFT VALIDATOR</span>";
		cmd +="<div id='doneID' class='done'>";
		cmd+="<a href='?m="+prev_menu+"'><img src='img/button_done.png' onclick='\"submitMEIDONE(\"998-VALIDATOR-DONE-'.which.'\"');' > </a>";
		cmd+="</div>";
		break;
	case "info":
       cmd="<span class='hdr'>INFO "+ which +" VALIDATOR</span>";
		break;
	case "reset":
       cmd="<span class='hdr'>RESETTING "+ which +" VALIDATOR</span>";
		break;
	case "verify":
       cmd="<span class='hdr'>VERIFYING BILLS</span>";
		break;
	}

	var id = fn;
	cmd += "<div class='data' id='"+id+"'><br></div>";
    cmd += "<div class='total' id='totalID'></div>";
	$(divID).html(cmd);
}


function setActionWhich(fn)
{
    // returns -1 if no match, else position
    if (fn.indexOf("left") != -1)  which="LEFT";
    if (fn.indexOf("right") != -1) which="RIGHT";


    if (fn.indexOf("stack") != -1)  action="stack";
    if (fn.indexOf("info") != -1)   action="info";
    if (fn.indexOf("reset") != -1)  action="reset";
    if (fn.indexOf("verify") != -1)	action="verify";
    if (fn.indexOf("done") != -1) 	action="done";

	// these need to be last beause "reset" above will find "utd_reset"
    if (fn.indexOf("utd_info") != -1)			action="utd_info";
    if (fn.indexOf("utd_reset") != -1)     		action="utd_reset";
    if (fn.indexOf("utd_inventory") != -1)		action="utd_inventory";


}


/*
	STARTING
	1. start the timeout timer
	2. reset timer when valid data is received from ajax_server
	3. start the interval timer that calls the AJAX function

	STOPPING
	1. stop the timeout timer
	2. stop the interval timer

*/

function startAJAX(theCaller)
{
//	alert('Starting AJAX');
	caller=theCaller;

	// set a 30 timeout for all AJAX activity
	timeouttimer = setTimeout(stopAJAX,TIMEOUT);
	callTimer = setInterval(getAJAX,INTERVAL);

}

/*
	stop and restart the timeout timer

	 reset when...
	1. a bill comes in (response from ajax_server)

*/

function resetTimeout()
{
    clearTimeout(timeouttimer);
    timeouttimer = setTimeout(stopAJAX,TIMEOUT);

}


/*
turn OFF the AJAX machine
*/

function stopAJAX()
{
	clearInterval(callTimer);
    clearTimeout(timeouttimer);
}


/*
	make an AJAX call to the server (ajax_server.php), every INTERVAL ms
	there is a timeout timer running that expired in 30s (configurable)

*/





function getAJAX()
{


// DEBUG: this worked
//	var data = { "action": "stack", "which" : "left" };

	// create an object
	var data={};
	// add  some stuff to it
	data.action=action;
	data.which=which;
//	var data = JSON.stringify(mydata);
//	alert("predata:" + data.action);


/*

	GREAT REFERENCE

	https://api.jquery.com/jquery.ajax/
*/

$.ajax(
 {
	type:"POST",		// alias for "method"
//	contentType: "application/json",	// using default: 'application/x-www-form-urlencoded; charset=UTF-8'
	dataType: "json",
	url: "ajax_server.php",
	cache: false,
	data: data,
	async: true,
	timeout: 100000,			// ms timeout value
	cache: false,
    success: function(data)
	{
//		alert("Data from the server " + data['json']);
		//Data from the server{USD:1000}
		var res = data['json'].split(":");
		// res[0] = USD
		// res[1] = 1000
//		alert(res[0] + "..." + res[1]);
		denom=res[0];
		totalStacked +=  parseInt(res[1],10);

		strng = $("#"+caller).html();		// get the current string


		switch(action)
		{
		case "done":
			stopAJAX();
			break;
		case "utd_info":
            var info= data['json'].split(":");
			Show_UTD_Info(info);
			stopAJAX();
			break;
		case "utd_reset":
            alert("UTD has been reset");
			stopAJAX();
			break;
		case "utd_inventory":
			// data is:: "0,0,0,0,0,0,0,0"
//			alert(data['json']);
			var inv = data['json'].split(",");
			Show_UTD_Inventory(inv);
			stopAJAX();
			break;
		case "stack":
			if (res[0] === "none") break;	// if no data ready
			resetTimeout();
			// returned string: USD:0100
			strng += "<br>Stacked one "+ res[1]/100 + " " + res[0] + " bill";
            $("#"+ caller).html(strng);

            // update the total
            var str="Total " + denom + " stacked:: "+ totalStacked/100;
            $("#totalID").html(str);

		// make sure last entry is visible
//		var elmnt = document.getElementById("mei_stack_left");
//		elmnt.scrollIntoView(false);

			break;
		case "info":
            if (res[0] === "none") break;   // if no data ready
			// Model- <info> :Serial-  <info>:
            strng = "<br>"+which+" VALIDATOR INFO<br>";
			for (n=0; n < res.length; n++)
				strng += "<br>"+res[n];

            $("#"+ caller).html(strng);
			stopAJAX();
			break;
		case "reset":
            strng += "<br>VALIDATOR RESET";
			strng +="<br>"+which+ " VALIDATOR WAS RESET";
            $("#"+ caller).html(strng);
			stopAJAX();
			break;
		case "verify":
            if (res[0] === "none") break;   // if no data ready
			resetTimeout();
            // returned string: USD:0100
			strng += "<br>Verified one "+ res[1]/100 + " " + res[0] + " bill";
            $("#"+ caller).html(strng);
			break;
		}

	},
   	error: function(e)
	{
		stopAJAX();
		alert('There was an error during the AJAX call!');
		console.log("ERROR : ", e);
		strng = $("#"+caller).html();
		strng += "<br> "+e.details;
		$("#"+caller).html(strng);
	}


 }); // end ajax
}	// end fn



/*
	display the returned inventory counts of the UTD

*/
function Show_UTD_Inventory(inv)
{
	var strng ="<table class='mtable'>";
	strng +="<tr><th>COL</th><th>COUNT</th></tr>";

	for (n=0; n < inv.length; n++)
		strng += "<tr><td>"+(n+1)+"</td><td>"+  inv[n] +"</td></tr>";


	strng +="</table>";
	$("#"+ caller).html(strng);

}

function Show_UTD_Info(info)
{
	var strng="<table class='mtable'>";
	strng +="<tr><td>"+ info[0]+"</td></tr>";
    strng +="<tr><td>"+ info[1]+"</td></tr>";


    strng +="</table>";
    $("#"+ caller).html(strng);


}



</script>

</html>


