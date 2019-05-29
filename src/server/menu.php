<?php

GLOBAL $conn;

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

if (extension_loaded('simplexml'))
	ConnectDBF();
else
	print "simplexml extension not loaded";



//https://dabuttonfactory.com/

if (isset($_GET['m']) )
{
	$menu=$_GET['m'];
	$function="";
}

if (isset($_GET['f']) )
{
	$function=$_GET['f'];
	$menu="none";
}


?>

<!DOCTYPE html>
<html lang="en-US">
<head>
  <title>VSAFE API</title>
  <meta charset="utf-8">
  <link rel="stylesheet" href="menu.css">
</head>
<body>

 <div class="topnav">
  <a href="?m=deposits">DEPOSITS</a>
  <a href="?m=withdrawl">WITHDRAWL</a>
  <a href="?m=reports">REPORTS</a>
  <a href="?m=settings">SETTINGS</a>
  <a href="?m=admin">ADMIN</a>
	<span class='cpr'>Copyright 2019 FireKing Secutiry Group, All Rights Reserved</span>
</div>

<?php


switch($menu)
{
case "none":
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



    print "</table>";


    print "</div>";

	break;
}


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
    print "</div>";
	VendInventory();
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
    print "</div>";
	VendUnload();
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

function UserTotals()
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
//	$sql="SELECT col,tube_name,tube_count,id FROM utd_denom";
	$sql="SELECT device_id,mfg,model FROM devices WHERE model='SCNL6627R'";
	$result = mysqli_query($conn, $sql);

	$row = mysqli_fetch_assoc($result);
	$utd_device_id = $row["device_id"];

	$sql="SELECT field_1,field_2,field_3,field_4,field_5,field_6 FROM inventory";
	$result= mysqli_query($conn,$sql);

}

/*
    $result = mysqli_query($conn,$sql);

    if (mysqli_num_rows >0)
    {
        while ($row = mysqli_fetch_assoc($result))
*/


function DepositCash()
{
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


function ConnectDBF()
{
	GLOBAL $conn;

	// our config file
    $configpath="/home/garyc/Desktop/VersaSafe/git/VersaSafe/src/xml/config.xml";
	// read it into $xml obj
    $xml=simplexml_load_file($configpath);

	//  print_rx($xml);

//	echo $xml->localDBF->ip;
	$dbf_server = $xml->localDBF->ip;
	$dbf_user= $xml->localDBF->user;
	$dbf_pw= $xml->localDBF->password;
	$dbf_dbf = $xml->localDBF->database;
	$dbf_port = $xml->localDBF->port;

	// connect to mySQL
	$conn = new mysqli($dbf_server,$dbf_user,$dbf_pw,$dbf_dbf);
	if ($conn->connect_error)
	{
	}
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


