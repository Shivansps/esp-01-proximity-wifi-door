<?php
$host    = "";
$user    = "";
$pass    = "";
$db_name = "";
$port 	 = "";

$key=$_POST["apikey"];
if(!isset($key))
	$key=$_GET["apikey"];

$getMacs=$_POST["GetMacs"];
$mac=$_GET["mac"];

if($key!="1234")
{
	echo "Incorrect API key";
	return;
}

$connection = mysqli_connect($host, $user, $pass, $db_name, $port);

if(mysqli_connect_errno()){
    die("connection failed: "
        . mysqli_connect_error()
        . " (" . mysqli_connect_errno()
        . ")");
}


$sql = "SELECT * FROM tb_macs";
$result = mysqli_query($connection, $sql);
$macs = "";
if($result->num_rows > 0)
{
	while($result2 = $result->fetch_assoc())
	{
		$macs .= $result2['mac'];
	}
}

if(isset($mac) && strpos($macs, $mac)===false)
{
	$sql = "INSERT INTO tb_macs (mac) VALUES  ( '".$mac."')";
	$result=mysqli_query($connection, $sql);
}

$connection->close();

if(isset($getMacs))
{
	echo strtolower($macs);
	return;
}

if(isset($mac))
{
	echo $mac;
	return;
}