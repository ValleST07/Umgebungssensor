const char* MAIN_page = R"=====(
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>BME280 Sensor</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            background: #f5f7fa; 
            margin: 0; 
            padding: 0; 
        }
        .container { 
            max-width: 1200px; 
            margin: 0 auto; 
            padding: 20px; 
        }
        header { 
            background: #4361ee; 
            color: white; 
            padding: 20px 0; 
            text-align: center; 
            border-radius: 0 0 10px 10px; 
        }
        .sensor-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); 
            gap: 15px; 
            margin: 20px 0; 
        }
        .card { 
            background: white; 
            border-radius: 8px; 
            padding: 15px; 
            box-shadow: 0 2px 5px rgba(0,0,0,0.1); 
        }
        .card h3 { 
            color: #4361ee; 
            margin-top: 0; 
        }
        .value { 
            font-size: 24px; 
            font-weight: bold; 
            margin: 10px 0; 
        }
        .unit { 
            color: #666; 
        }
        form { 
            background: white; 
            padding: 20px; 
            border-radius: 8px; 
            box-shadow: 0 2px 5px rgba(0,0,0,0.1); 
            margin-bottom: 20px; 
        }
        input[type='text'], 
        input[type='password'], 
        input[type='file'] { 
            width: 100%; 
            padding: 10px; 
            margin: 5px 0 15px; 
            border: 1px solid #ddd; 
            border-radius: 4px; 
        }
        button { 
            background: #4361ee; 
            color: white; 
            border: none; 
            padding: 10px 15px; 
            border-radius: 4px; 
            cursor: pointer; 
        }
        button:hover { 
            background: #3a56d4; 
        }
        .chart-container {
            background: white;
            border-radius: 8px;
            padding: 15px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            margin-bottom: 20px;
        }
        @media (max-width: 600px) { 
            .sensor-grid { 
                grid-template-columns: 1fr; 
            } 
        }
        .github-corner { 
            position: absolute; 
            top: 0; 
            right: 0; 
            border: 0; 
        }
        .github-corner:hover .octo-arm { 
            animation: octocat-wave 560ms ease-in-out; 
        }
        @keyframes octocat-wave { 
            0%,100% { transform: rotate(0); } 
            20%,60% { transform: rotate(-25deg); } 
            40%,80% { transform: rotate(10deg); } 
        }
        @media (max-width:500px) { 
            .github-corner:hover .octo-arm { 
                animation: none; 
            } 
            .github-corner .octo-arm { 
                animation: octocat-wave 560ms ease-in-out; 
            } 
        }
    </style>
    <!-- Chart.js CDN -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <div class="container">
        <a href="https://github.com/ValleST07/Umgebungssensor" class="github-corner" target="_blank" aria-label="View source on GitHub">
            <svg width="80" height="80" viewBox="0 0 250 250" style="fill:#151513; color:#fff;">
                <path d="M0,0 L115,115 L130,115 L142,142 L250,250 L250,0 Z"></path>
                <path d="M128.3,109.0 C113.8,99.7 119.0,89.6 119.0,89.6 C122.0,82.7 120.5,78.6 120.5,78.6 C119.2,72.0 123.4,76.3 123.4,76.3 C127.3,80.9 125.5,87.3 125.5,87.3 C122.9,97.6 130.6,101.9 134.4,103.2" fill="currentColor" style="transform-origin: 130px 106px;" class="octo-arm"></path>
                <path d="M115.0,115.0 C114.9,115.1 118.7,116.5 119.8,115.4 L133.7,101.6 C136.9,99.2 139.9,98.4 142.2,98.6 C133.8,88.0 127.5,74.4 143.8,58.0 C148.5,53.4 154.0,51.2 159.7,51.0 C160.3,49.4 163.2,43.6 171.4,40.1 C171.4,40.1 176.1,42.5 178.8,56.2 C183.1,58.6 187.2,61.8 190.9,65.4 C194.5,69.0 197.7,73.2 200.1,77.6 C213.8,80.2 216.3,84.9 216.3,84.9 C212.7,93.1 206.9,96.0 205.4,96.6 C205.1,102.4 203.0,107.8 198.3,112.5 C181.9,128.9 168.3,122.5 157.7,114.1 C157.9,116.9 156.7,120.9 152.7,124.9 L141.0,136.5 C139.8,137.7 141.6,141.9 141.8,141.8 Z" fill="currentColor" class="octo-body"></path>
            </svg>
        </a>

        <header>
            <h1>BME280 Sensor</h1>
            <p>Echtzeit-Daten</p>
        </header>

        <div class="sensor-grid">
            <div class="card">
                <h3>Temperatur</h3>
                <div class="value" id="temp">0.0</div>
                <div class="unit">°C</div>
            </div>
            <div class="card">
                <h3>Luftfeuchtigkeit</h3>
                <div class="value" id="hum">0.0</div>
                <div class="unit">%</div>
            </div>
            <div class="card">
                <h3>Druck</h3>
                <div class="value" id="press">0.0</div>
                <div class="unit">hPa</div>
            </div>
            <div class="card">
                <h3>Höhe</h3>
                <div class="value" id="alt">0.0</div>
                <div class="unit">m</div>
            </div>
        </div>

        <div class="chart-container">
    <h2>Sensorverlauf</h2>
    <div id="chart-unavailable" style="display: none; color: red; font-weight: bold;">
        Diagramm nicht verfügbar. Bitte stellen Sie eine Internetverbindung her, um es anzuzeigen.
    </div>
    <canvas id="sensorChart"></canvas>
</div>


        <form method="POST" action="/config">
            <h2>WLAN-Konfiguration</h2>
            <label for="ssid">SSID:</label>
            <input type="text" name="ssid" placeholder="WLAN-Name">
            <label for="password">Passwort:</label>
            <input type="password" name="password" placeholder="WLAN-Passwort">
            <button type="submit">Einstellungen speichern</button>
            <p style="font-size: 12px; color: #666;">Leer lassen und Speichern für AP-Modus</p>
        </form>

        <form method="POST" action="/update" enctype="multipart/form-data">
            <h2>Firmware-Update</h2>
            <input type="file" name="update">
            <button type="submit">Hochladen & Installieren</button>
        </form>

        <footer style="text-align:center;margin-top:30px;color:#666;font-size:14px;">
            <a href="https://github.com/ValleST07/Umgebungssensor" target="_blank" style="color:#4361ee;text-decoration:none;">
                View project on GitHub
            </a>
        </footer>
    </div>
     <script>
        setInterval(() => {
            fetch('/data')
                .then(r => r.json())
                .then(d => {
                    document.getElementById('temp').innerText = d.temp.toFixed(1);
                    document.getElementById('hum').innerText = d.hum.toFixed(1);
                    document.getElementById('press').innerText = d.press.toFixed(1);
                    document.getElementById('alt').innerText = d.press.toFixed(1);
                });
        }, 2000);
    </script>
    <script>
    // Prüfen ob Chart.js geladen ist
    if (typeof Chart === 'undefined') {
        document.getElementById('sensorChart').style.display = 'none';
        document.getElementById('chart-unavailable').style.display = 'block';
    } else {
        const ctx = document.getElementById('sensorChart').getContext('2d');
        const chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [
                    {
                        label: 'Temperatur (°C)',
                        data: [],
                        borderColor: 'rgb(255, 99, 132)',
                        backgroundColor: 'rgba(255, 99, 132, 0.1)',
                        tension: 0.1,
                        yAxisID: 'y'
                    },
                    {
                        label: 'Luftfeuchtigkeit (%)',
                        data: [],
                        borderColor: 'rgb(54, 162, 235)',
                        backgroundColor: 'rgba(54, 162, 235, 0.1)',
                        tension: 0.1,
                        yAxisID: 'y1'
                    },
                    {
                        label: 'Druck (hPa)',
                        data: [],
                        borderColor: 'rgb(75, 192, 192)',
                        backgroundColor: 'rgba(75, 192, 192, 0.1)',
                        tension: 0.1,
                        yAxisID: 'y2'
                    }
                ]
            },
            options: {
                responsive: true,
                interaction: {
                    mode: 'index',
                    intersect: false,
                },
                scales: {
                    y: {
                        type: 'linear',
                        display: true,
                        position: 'left',
                        title: {
                            display: true,
                            text: 'Temperatur (°C)'
                        }
                    },
                    y1: {
                        type: 'linear',
                        display: true,
                        position: 'right',
                        grid: {
                            drawOnChartArea: false,
                        },
                        title: {
                            display: true,
                            text: 'Luftfeuchtigkeit (%)'
                        },
                        min: 0,
                        max: 100
                    },
                    y2: {
                        type: 'linear',
                        display: true,
                        position: 'right',
                        grid: {
                            drawOnChartArea: false,
                        },
                        title: {
                            display: true,
                            text: 'Druck (hPa)'
                        }
                    }
                }
            }
        });

        const maxDataPoints = 20;
        const timeLabels = [];
        const tempData = [];
        const humData = [];
        const pressData = [];

        function updateChart(time, temp, hum, press) {
            timeLabels.push(time);
            if (timeLabels.length > maxDataPoints) timeLabels.shift();

            tempData.push(temp);
            humData.push(hum);
            pressData.push(press);

            if (tempData.length > maxDataPoints) {
                tempData.shift();
                humData.shift();
                pressData.shift();
            }

            chart.data.labels = timeLabels;
            chart.data.datasets[0].data = tempData;
            chart.data.datasets[1].data = humData;
            chart.data.datasets[2].data = pressData;
            chart.update();
        }

        setInterval(() => {
            fetch('/data')
                .then(r => r.json())
                .then(d => {
                    const now = new Date();
                    const timeString = now.toLocaleTimeString();

                    document.getElementById('temp').innerText = d.temp.toFixed(1);
                    document.getElementById('hum').innerText = d.hum.toFixed(1);
                    document.getElementById('press').innerText = d.press.toFixed(1);
                    document.getElementById('alt').innerText = d.alt.toFixed(1);

                    updateChart(timeString, d.temp, d.hum, d.press);
                });
        }, 2000);
    }
</script>

</body>
</html>
)=====";