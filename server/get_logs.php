<?php
header('Content-Type: application/json');

// --- Database_Config ---
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "rfid_system";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    echo json_encode([]);
    exit();
}

// Take last 10 log, ordered from the newest
$sql = "SELECT uid, status, log_time FROM rfid_logs ORDER BY log_time DESC LIMIT 10";
$result = $conn->query($sql);

$logs = [];
if ($result && $result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $logs[] = $row;
    }
}

$conn->close();

echo json_encode($logs);
?>
