<?php
// Start the session
session_start();

$valid_username = "admin";
$valid_password = "password123";

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $username = $_POST['username'];
    $password = $_POST['password'];

    echo "-------------";
    echo $username;
    echo $password;
    
    if ($username == $valid_username && $password == $valid_password) {

        $_SESSION['username'] = $username;
        echo "<h3>Welcome, " . $username . "!</h3>";
        echo "<p><a href='logout.php'>Logout</a></p>";
    } else {
        echo "<p style='color: red;'>Invalid username or password. Please try again.</p>";
    }
} 
else {
    echo "<p>Please fill in the login form.</p>";
}
?>
