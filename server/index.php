<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Log UID RFID</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;600&display=swap" rel="stylesheet">
    <style>
        body { font-family: 'Inter', sans-serif; }
    </style>
</head>
<body class="bg-slate-100 p-8">

    <div class="max-w-4xl mx-auto bg-white rounded-xl shadow-lg p-6">
        <h1 class="text-3xl font-bold text-center text-slate-800 mb-6">LOG UID RFID</h1>
        
        <div id="uid-log" class="space-y-4">
            <!-- Log UID will be loaded by JavaScript -->
            <div class="p-4 bg-slate-50 text-slate-500 text-center font-semibold rounded-md animate-pulse">Loading...</div>
        </div>
    </div>

    <script>
        async function fetchLogs() {
            try {
                const response = await fetch('http://localhost/rfid_web/get_logs.php'); 
                const logs = await response.json();
                const logContainer = document.getElementById('uid-log');
                logContainer.innerHTML = '';

                if (logs.length > 0) {
                    logs.forEach(log => {
                        const logDiv = document.createElement('div');
                        const statusColor = log.status === 'ALLOWED' ? 'bg-green-100 border-green-500' : 'bg-red-100 border-red-500';
                        const statusText = log.status === 'ALLOWED' ? '✅ Access Granted' : '❌ Access Denied';
                        const uidColor = log.status === 'ALLOWED' ? 'text-green-700' : 'text-red-700';
                        
                        logDiv.className = `p-4 border-l-4 rounded-md shadow-sm ${statusColor}`;
                        logDiv.innerHTML = `
                            <p class="font-bold text-lg ${uidColor}">${log.uid}</p>
                            <p class="text-sm text-slate-600 mt-1">${statusText}</p>
                            <p class="text-xs text-slate-400 mt-0.5">${log.log_time}</p>
                        `;
                        logContainer.appendChild(logDiv);
                    });
                } else {
                    logContainer.innerHTML = '<div class="p-4 bg-slate-50 text-slate-500 text-center font-semibold rounded-md">UID data is empty.</div>';
                }
            } catch (error) {
                const logContainer = document.getElementById('uid-log');
                logContainer.innerHTML = `<div class="p-4 bg-red-100 text-red-700 font-semibold rounded-md">Error loading the data: ${error.message}</div>`;
            }
        }

        fetchLogs();
        setInterval(fetchLogs, 2000);
    </script>
</body>
</html>
