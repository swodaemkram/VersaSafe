<?php
session_start();    // start a user session
//echo session_id();
//print_rx($_SESSION);
//print_rx($_POST);

?>

<html>
<head>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
</head>

<?php
/*
	File: menu.php
	Author: Gary Conway <gary.conway@fireking.com>
	Created: 5-25-2019
	Updated:


	This is a GUI designed to interact with the VSAFE API
*/


GLOBAL $conn,$xml,$cfg,$socket,$port, $host, $api_connected, $input_vars;

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
    "api_port"=>6,
);


ini_set('html_errors',1);
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

ReadXML();

get_input_vars();	// place all GET|POST vars in $input_vars array
var_dump($input_vars);

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

/*
if (isset($_GET['m']) )
{
	$menu=$_GET['m'];
	$function="";
}
else
	$menu="main";
	$function="";

if (isset($_GET['f']) )
{
	$function=$_GET['f'];
	$menu="none";
}


if (isset($_POST['f']) )
{
	$function = $_POST['f'];
}
*/

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
    print "<th/>";
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
    $ret=SocketConnect();
	if ($ret)
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
    print "<td>";
	print "</td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?m=main'><img src='img/button_done.png' /> </a> </td>";
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
			while ($waiting AND (time() - $start_time <30) )
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
	print "<th/>";
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
    print "<th/>";
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
    print "SETTINGS";
    print "</th>";
    print "</tr>";

    print "<tr>";
    print "<th/>";
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
    print "<td><a href='?f=vend_inventory'><img src='img/button_vend-inventory.png' /> </a></td>";
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
    print "<th/>";
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
    print "<th/>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=perform_eod'><img src='img/button_perform-eod.png' /> </a></td>";
    print "<td><a href='?f=content_removal'><img src='img/button_content-removal.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=maint'><img src='img/button_maint.png' /> </a></td>";
    print "<td><a href='?f=manage_users'><img src='img/button_manage-users.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=chg_order'><img src='img/button_chg-order.png' /> </a></td>";
    print "<td><a href='?f=vend_load'><img src='img/button_vend-load.png' /> </a></td>";
    print "</tr>";

    print "<tr>";
    print "<td><a href='?f=vend_unload'><img src='img/button_vend-unload.png' /> </a></td>";
    print "<td><a href='?f=vend_define'><img src='img/button_vend-define.png' /> </a></td>";
    print "</tr>";

/*
    print "<tr>";
    print "<td><a href='?f=vend_unload_all'><img src='img/button_unload-all.png' /> </a></td>";
    print "</tr>";
*/


    print "</table>";


    print "</div>";

	break;
}



//========================================================
//					FUNCTIONS
//========================================================
switch($function)
{



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

case "maint":
    print "<div class='function'>";
    print "</div>";
	Maint();
	break;

case "vend_unload":
    print "<div class='function'>";

	print "<form id='unloadformID' action='". htmlspecialchars($_SERVER["PHP_SELF"]) ."' method='post'>";

	print "<input type='hidden' id='fn' name='f' value=''>";
	print "<table class='columns'>\n";


        print "<tr>\n";
        print "<th colspan='2'>VEND UNLOAD</th>";
        print "</tr>\n";

        print "<tr>";
		print "<th>COLUMN</th><th>COLUMN</th>";
        print "</tr>\n";


	for ($n=1; $n <5; $n++)
	{
		print "<tr>\n";
		print "<td><input type='checkbox' name='col".$n ."' value='". $n ."'>".$n."</td>\n";
        print "<td><input type='checkbox' name='col". ($n+4) ."' value='". ($n+4) ."'>". ($n+4)."</td>\n";
	    print "</tr>\n";
	}

	print "<tr>\n";
	print "<td><button class='unloadnow' id='unloadnow' onclick='submitUNLOAD(\"vend_unload_now\");'><img src='img/button_unload-now.png'/></button></td>";
    print "<td><button class='unloadall' id='unloadall' onclick='submitUNLOAD(\"vend_unload_all\");'><img src='img/button_unload-all.png'/></button></td>";
    print "</tr>\n";

    print "<tr>";
    print "<td><br><br><a href='?m=admin'><img src='img/button_cancel.png' /> </a></td>";
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
	$cfg["api_port"]=$xml->api->port;

	set_time_limit(0);	// no time out


	$port = (int) $cfg["api_port"];

	// create a socket
    // returns a socket resource on success, else false on error
	$socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");

	if (! $socket)
	{
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        socket_clear_error();
        return $errormsg;
	}

	// connect to the server
	// returns true on success, else false
	$result = socket_connect($socket, $host, $port) or die("Could not connect toserver\n");

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


/*
	read a message from the API
	RETURNS: a message from the API or an error message
			returns "" is there is no data ready on the port
*/

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
    function submitUNLOAD(fn)
	{
        alert(fn);
		$("#fn").val(fn);
//		e.preventDefault();
       $("#unloadformID").submit();
    }


	function submitLOGIN(fn)
	{
		$("#fnlogin").val(fn);
		$("#loginformID").submit();
	}




</script>

</html>


