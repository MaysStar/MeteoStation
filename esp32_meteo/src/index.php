<?php
session_start();

// Ініціалізація історії даних у сесії
if (!isset($_SESSION['data_history'])) {
    $_SESSION['data_history'] = [];
}

// Обробка POST-запитів від ESP32
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true);
    if ($input) {
        $entry = [
            'time' => date('H:i:s'),
            'temperature' => floatval($input['temperature'] ?? 0),
            'humidity' => floatval($input['humidity'] ?? 0),
            'pressure' => floatval($input['pressure'] ?? 0)
        ];
        $_SESSION['data_history'][] = $entry;
        // Максимум 100 записів
        if (count($_SESSION['data_history']) > 100) {
            array_shift($_SESSION['data_history']);
        }
        echo json_encode(['status' => 'ok']);
        exit;
    }
}

// JSON для JS-графіків
if (isset($_GET['json'])) {
    $times = array_column($_SESSION['data_history'], 'time');
    $temps = array_column($_SESSION['data_history'], 'temperature');
    $hums  = array_column($_SESSION['data_history'], 'humidity');
    $press = array_column($_SESSION['data_history'], 'pressure');
    header('Content-Type: application/json');
    echo json_encode([
        'times' => $times,
        'temperatures' => $temps,
        'humidities' => $hums,
        'pressures' => $press
    ]);
    exit;
}

// Весь HTML та JS для Dashboard нижче
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>ESP32 Sensor Dashboard</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
body { font-family: 'Segoe UI', sans-serif; background: #0f172a; color:#f1f5f9; margin:0; padding:20px; }
h1{text-align:center;}
.cards { display:flex; gap:20px; justify-content:center; flex-wrap:wrap; margin-bottom:40px; }
.card { background:#334155; padding:20px; border-radius:16px; width:200px; text-align:center; box-shadow:0 5px 15px rgba(0,0,0,0.3);}
.card h2{margin:10px 0;}
.chart-container { background:#334155; padding:20px; border-radius:16px; box-shadow:0 5px 15px rgba(0,0,0,0.3); margin-bottom:40px;}
canvas{background:#1e293b; border-radius:16px;}
table{width:100%; border-collapse:collapse; margin-top:20px;}
th,td{border:1px solid #475569; padding:8px; text-align:center;}
th{background:#475569;}
</style>
</head>
<body>

<h1>🌡️ ESP32 Sensor Dashboard</h1>

<div class="cards">
    <div class="card"><h2 id="temp-current">-- °C</h2>Temperature</div>
    <div class="card"><h2 id="hum-current">-- %</h2>Humidity</div>
    <div class="card"><h2 id="pressure-current">-- hPa</h2>Pressure</div>
</div>

<div class="chart-container">
    <canvas id="chart-temp"></canvas>
</div>
<div class="chart-container">
    <canvas id="chart-hum"></canvas>
</div>
<div class="chart-container">
    <canvas id="chart-press"></canvas>
</div>

<h2>Historical Data</h2>
<table>
<tr><th>Time</th><th>Temperature (°C)</th><th>Humidity (%)</th><th>Pressure (hPa)</th></tr>
<?php foreach($_SESSION['data_history'] as $entry): ?>
<tr>
<td><?= $entry['time'] ?></td>
<td><?= $entry['temperature'] ?></td>
<td><?= $entry['humidity'] ?></td>
<td><?= $entry['pressure'] ?></td>
</tr>
<?php endforeach; ?>
</table>

<script>
const ctxTemp = document.getElementById('chart-temp').getContext('2d');
const ctxHum  = document.getElementById('chart-hum').getContext('2d');
const ctxPress= document.getElementById('chart-press').getContext('2d');

const chartTemp = new Chart(ctxTemp, {
    type:'line',
    data:{ labels:[], datasets:[{label:'Temperature (°C)', data:[], borderColor:'#ef4444', backgroundColor:'rgba(239,68,68,0.2)'}]},
    options:{ responsive:true }
});
const chartHum = new Chart(ctxHum, {
    type:'line',
    data:{ labels:[], datasets:[{label:'Humidity (%)', data:[], borderColor:'#06b6d4', backgroundColor:'rgba(6,182,212,0.2)'}]},
    options:{ responsive:true }
});
const chartPress = new Chart(ctxPress, {
    type:'line',
    data:{ labels:[], datasets:[{label:'Pressure (hPa)', data:[], borderColor:'#f59e0b', backgroundColor:'rgba(245,158,11,0.2)'}]},
    options:{ responsive:true }
});

// Автооновлення кожні 2 секунди
async function updateData(){
    const res = await fetch('?json=1');
    const data = await res.json();
    chartTemp.data.labels = data.times;
    chartTemp.data.datasets[0].data = data.temperatures;
    chartHum.data.labels  = data.times;
    chartHum.data.datasets[0].data = data.humidities;
    chartPress.data.labels= data.times;
    chartPress.data.datasets[0].data = data.pressures;
    chartTemp.update(); chartHum.update(); chartPress.update();

    if(data.times.length>0){
        document.getElementById('temp-current').textContent = data.temperatures.slice(-1)[0].toFixed(2)+' °C';
        document.getElementById('hum-current').textContent = data.humidities.slice(-1)[0].toFixed(2)+' %';
        document.getElementById('pressure-current').textContent = data.pressures.slice(-1)[0].toFixed(2)+' hPa';
    }
}
setInterval(updateData, 2000);
updateData();
</script>

</body>
</html>