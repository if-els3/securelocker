<?php
header('Content-Type: application/json');

$response = ["status" => "DENY"];

// --- Konfigurasi Database ---
$servername = "localhost";
$username = "root"; 
$password = ""; 
$dbname = "rfid_system";


$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    echo json_encode(["status" => "ERROR", "message" => "Connection failed: " . $conn->connect_error]);
    exit();
}

// --- Konfigurasi AES-128 ---
// key must precise as in the ESP32 and must 16 byte
$encryption_key = 'kunci_rahasia_1234'; 
$iv = '1234567890123456'; // Initialization Vector must 16 byte

// Take encrypted data from ESP32
$input_data = file_get_contents('php://input');
$data = json_decode($input_data, true);

if (isset($data['encrypted_uid']) && isset($data['iv'])) {
    $encrypted_uid_b64 = $data['encrypted_uid'];
    $received_iv_b64 = $data['iv'];

    $encrypted_uid = base64_decode($encrypted_uid_b64);
    $received_iv = base64_decode($received_iv_b64);
    
    //decrypt uid data
    $decrypted_uid = openssl_decrypt(
        $encrypted_uid, 
        'aes-128-cbc', 
        $encryption_key, 
        OPENSSL_RAW_DATA, 
        $received_iv
    );
    
    if ($decrypted_uid === false) {
        echo json_encode(["status" => "ERROR", "message" => "Decryption failed"]);
        exit();
    }
    

    $uid_from_esp = trim($decrypted_uid);
    $access_status = "DENIED";

    //check is the uid is allowed from 'allowed_uids'
    $sql_check = "SELECT uid FROM allowed_uids WHERE uid = '{$uid_from_esp}'";
    $result = $conn->query($sql_check);
    
    if ($result && $result->num_rows > 0) {
        $access_status = "ALLOWED";
        $response["status"] = "ALLOW"; // Kirim "ALLOW" ke ESP32
    }

    // input the log to `rfid_logs`
    $sql_log = "INSERT INTO rfid_logs (uid, status) VALUES ('{$uid_from_esp}', '{$access_status}')";
    $conn->query($sql_log);
}

$conn->close();

echo json_encode($response);
?>
