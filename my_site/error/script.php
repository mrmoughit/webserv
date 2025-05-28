<?php
// Infinite loop that prints a message every second
while (true {
    echo "Still running...\n";
    flush(); // Ensure output is sent immediately
    sleep(1); // Pause for 1 second to avoid overwhelming the server
}
?>
