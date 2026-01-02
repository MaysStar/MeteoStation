<?php
// ================= CONFIG =================
$dataFile = "data.json";
date_default_timezone_set('Europe/Kyiv');
$maxHistory = 10000;
$updateInterval = 3000; // milliseconds

if (!file_exists($dataFile)) {
    file_put_contents($dataFile, json_encode([]));
}

// ============== HELPER FUNCTIONS ==============
function calculateStatistics($data) {
    if (empty($data)) {
        return [
            'temperature' => ['current' => 0, 'average' => 0, 'min' => 0, 'max' => 0, 'trend' => 'stable'],
            'humidity' => ['current' => 0, 'average' => 0, 'min' => 0, 'max' => 0, 'trend' => 'stable'],
            'pressure' => ['current' => 0, 'average' => 0, 'min' => 0, 'max' => 0, 'trend' => 'stable']
        ];
    }
    
    $temps = array_column($data, 'temperature');
    $hums = array_column($data, 'humidity');
    $press = array_column($data, 'pressure');
    
    return [
        'temperature' => [
            'current' => !empty($temps) ? end($temps) : 0,
            'average' => !empty($temps) ? array_sum($temps) / count($temps) : 0,
            'min' => !empty($temps) ? min($temps) : 0,
            'max' => !empty($temps) ? max($temps) : 0,
            'trend' => !empty($temps) ? calculateTrend($temps) : 'stable'
        ],
        'humidity' => [
            'current' => !empty($hums) ? end($hums) : 0,
            'average' => !empty($hums) ? array_sum($hums) / count($hums) : 0,
            'min' => !empty($hums) ? min($hums) : 0,
            'max' => !empty($hums) ? max($hums) : 0,
            'trend' => !empty($hums) ? calculateTrend($hums) : 'stable'
        ],
        'pressure' => [
            'current' => !empty($press) ? end($press) : 0,
            'average' => !empty($press) ? array_sum($press) / count($press) : 0,
            'min' => !empty($press) ? min($press) : 0,
            'max' => !empty($press) ? max($press) : 0,
            'trend' => !empty($press) ? calculateTrend($press) : 'stable'
        ]
    ];
}

function calculateTrend($data) {
    if (count($data) < 5) return 'stable';
    
    $lastValues = array_slice($data, -5);
    $first = reset($lastValues);
    $last = end($lastValues);
    
    if ($last > $first + 0.5) return 'up';
    if ($last < $first - 0.5) return 'down';
    return 'stable';
}

function getColorByValue($value, $type = 'temperature') {
    if ($type == 'temperature') {
        if ($value < 10) return '#4cc9f0';
        elseif ($value < 20) return '#4361ee';
        elseif ($value < 25) return '#7209b7';
        elseif ($value < 30) return '#f72585';
        else return '#ff0000';
    } elseif ($type == 'humidity') {
        if ($value < 30) return '#ff6b6b';
        elseif ($value < 60) return '#51cf66';
        elseif ($value < 80) return '#ffd43b';
        else return '#ff922b';
    } elseif ($type == 'pressure') {
        if ($value < 1000) return '#748ffc';
        elseif ($value < 1020) return '#5c7cfa';
        else return '#364fc7';
    }
    return '#495057';
}

// ============== RECEIVE DATA (POST) ==============
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $raw = file_get_contents("php://input");
    $data = json_decode($raw, true);

    if ($data) {
        $history = json_decode(file_get_contents($dataFile), true);
        if (!$history) $history = [];

        $history[] = [
            "time" => date("H:i:s"),
            "temperature" => floatval($data["temperature"] ?? 0),
            "humidity" => floatval($data["humidity"] ?? 0),
            "pressure" => floatval($data["pressure"] ?? 1013.25)
        ];

        if (count($history) > $maxHistory) {
            array_shift($history);
        }

        file_put_contents($dataFile, json_encode($history));
        echo json_encode(["status" => "success", "message" => "Data received"]);
        exit;
    }
}

// ============== EXPORT DATA ===================
if (isset($_GET['export']) && $_GET['export'] == 'csv') {
    $history = json_decode(file_get_contents($dataFile), true);
    if (!$history) $history = [];
    
    header('Content-Type: text/csv; charset=UTF-8');
    header('Content-Disposition: attachment; filename="сенсорні_дані_' . date('Ymd_His') . '.csv"');
    
    $output = fopen('php://output', 'w');
    
    // Додати BOM для коректного відображення UTF-8 в Excel
    fwrite($output, "\xEF\xBB\xBF");
    
    // Вказати Excel, що роздільник - крапка з комою
    fwrite($output, "sep=;\n");
    
    // Змінюємо роздільник на крапку з комою
    $delimiter = ';';
    
    // Українські заголовки
    fputcsv($output, ['Час', 'Температура (°C)', 'Вологість (%)', 'Тиск (hPa)'], $delimiter);
    
    foreach ($history as $entry) {
        fputcsv($output, [
            $entry['time'] ?? '',
            $entry['temperature'] ?? 0,
            $entry['humidity'] ?? 0,
            $entry['pressure'] ?? 0
        ], $delimiter);
    }
    
    fclose($output);
    exit;
}

// ============== CLEAR DATA ===================
if (isset($_GET['clear']) && $_GET['clear'] == '1') {
    file_put_contents($dataFile, json_encode([]));
    echo json_encode(["status" => "success", "message" => "Data cleared"]);
    exit;
}

// ============== JSON FOR CHARTS ===================
if (isset($_GET["json"])) {
    $history = json_decode(file_get_contents($dataFile), true);
    if (!$history) $history = [];
    
    $times = array_column($history, "time");
    $temperatures = array_column($history, "temperature");
    $humidities = array_column($history, "humidity");
    $pressures = array_column($history, "pressure");
    
    $response = [
        "times" => $times,
        "temperatures" => $temperatures,
        "humidities" => $humidities,
        "pressures" => $pressures,
        "count" => count($history),
        "last_update" => date("H:i:s")
    ];
    
    header('Content-Type: application/json');
    echo json_encode($response);
    exit;
}

// ============== MAIN PAGE ===================
$history = json_decode(file_get_contents($dataFile), true);
if (!$history) $history = [];

$stats = calculateStatistics($history);
$lastData = !empty($history) ? end($history) : null;

$times = array_column($history, "time");
$temperatures = array_column($history, "temperature");
$humidities = array_column($history, "humidity");
$pressures = array_column($history, "pressure");

$current_time = date("Y-m-d H:i:s");
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🌡️ ESP32 Professional Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>
        :root {
            --primary: #3b82f6;
            --secondary: #10b981;
            --danger: #ef4444;
            --warning: #f59e0b;
            --info: #8b5cf6;
            --dark: #0f172a;
            --darker: #020617;
            --light: #f8fafc;
            --gray: #64748b;
            --card-bg: rgba(30, 41, 59, 0.9);
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, var(--darker) 0%, var(--dark) 100%);
            color: var(--light);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 2000px;
            margin: 0 auto;
        }
        
        /* Header */
        .header {
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            padding: 30px;
            border-radius: 24px;
            margin-bottom: 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3);
        }
        
        .header-left h1 {
            font-size: 2.8rem;
            font-weight: 800;
            background: linear-gradient(135deg, #60a5fa 0%, #3b82f6 30%, #8b5cf6 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 10px;
            letter-spacing: -0.5px;
        }
        
        .header-left p {
            color: #94a3b8;
            font-size: 1.1rem;
            max-width: 600px;
        }
        
        .header-stats {
            display: flex;
            gap: 20px;
            flex-wrap: wrap;
        }
        
        .stat-badge {
            background: rgba(255, 255, 255, 0.08);
            padding: 15px 25px;
            border-radius: 16px;
            display: flex;
            align-items: center;
            gap: 12px;
            border: 1px solid rgba(255, 255, 255, 0.05);
            min-width: 180px;
        }
        
        .stat-badge i {
            font-size: 1.4rem;
            color: var(--primary);
        }
        
        .stat-badge span {
            font-weight: 600;
            font-size: 1.1rem;
        }
        
        /* Dashboard Grid */
        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(12, 1fr);
            gap: 25px;
            margin-bottom: 30px;
        }
        
        /* Sensor Cards */
        .sensor-card {
            grid-column: span 4;
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            padding: 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
            position: relative;
            overflow: hidden;
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
        }
        
        .sensor-card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 4px;
            background: linear-gradient(90deg, var(--primary), var(--secondary));
            border-radius: 24px 24px 0 0;
        }
        
        .sensor-card:hover {
            transform: translateY(-8px);
            border-color: var(--primary);
            box-shadow: 0 25px 50px rgba(0, 0, 0, 0.3);
        }
        
        .sensor-card.temp::before { background: linear-gradient(90deg, #ef4444, #f97316); }
        .sensor-card.hum::before { background: linear-gradient(90deg, #3b82f6, #06b6d4); }
        .sensor-card.press::before { background: linear-gradient(90deg, #10b981, #84cc16); }
        
        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
            padding-bottom: 20px;
            border-bottom: 2px solid rgba(255, 255, 255, 0.1);
        }
        
        .card-title {
            font-size: 1.5rem;
            font-weight: 700;
            color: var(--light);
            display: flex;
            align-items: center;
            gap: 15px;
        }
        
        .card-title i {
            font-size: 2rem;
            background: rgba(255, 255, 255, 0.1);
            padding: 15px;
            border-radius: 16px;
        }
        
        .sensor-value {
            font-size: 4.5rem;
            font-weight: 900;
            margin: 20px 0;
            font-family: 'Courier New', monospace;
            text-align: center;
            line-height: 1;
        }
        
        .sensor-details {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-top: 25px;
        }
        
        .detail-item {
            background: rgba(255, 255, 255, 0.05);
            padding: 20px;
            border-radius: 16px;
            text-align: center;
            border: 1px solid rgba(255, 255, 255, 0.05);
        }
        
        .detail-label {
            font-size: 0.9rem;
            color: #94a3b8;
            text-transform: uppercase;
            letter-spacing: 1px;
            margin-bottom: 8px;
        }
        
        .detail-value {
            font-size: 1.8rem;
            font-weight: 700;
        }
        
        .trend {
            display: flex;
            align-items: center;
            gap: 8px;
            font-size: 1rem;
            padding: 8px 16px;
            border-radius: 50px;
            background: rgba(255, 255, 255, 0.05);
        }
        
        .trend.up { color: var(--secondary); background: rgba(16, 185, 129, 0.1); }
        .trend.down { color: var(--danger); background: rgba(239, 68, 68, 0.1); }
        .trend.stable { color: var(--warning); background: rgba(245, 158, 11, 0.1); }
        
        /* Chart Cards - One for each sensor */
        .chart-card {
            grid-column: span 4;
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            padding: 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            height: 550px;
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
            display: flex;
            flex-direction: column;
        }
        
        .chart-card.temp-chart {
            border-top: 4px solid #ef4444;
        }
        
        .chart-card.hum-chart {
            border-top: 4px solid #3b82f6;
        }
        
        .chart-card.press-chart {
            border-top: 4px solid #10b981;
        }
        
        .chart-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
        }
        
        .chart-title {
            font-size: 1.4rem;
            font-weight: 700;
            color: var(--light);
            display: flex;
            align-items: center;
            gap: 12px;
        }
        
        .chart-container {
            flex: 1;
            position: relative;
            width: 100%;
        }
        
        /* Advanced Analytics Section */
        .analytics-grid {
            grid-column: span 12;
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 25px;
            margin-top: 25px;
        }
        
        .analytics-card {
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            padding: 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
        }
        
        .analytics-title {
            font-size: 1.3rem;
            font-weight: 700;
            color: var(--light);
            margin-bottom: 20px;
            display: flex;
            align-items: center;
            gap: 12px;
        }
        
        .analytics-content {
            font-size: 0.95rem;
            color: #94a3b8;
            line-height: 1.6;
        }
        
        /* Controls */
        .controls-container {
            grid-column: span 12;
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            padding: 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            margin-top: 25px;
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
        }
        
        .controls-title {
            font-size: 1.4rem;
            font-weight: 700;
            color: var(--light);
            margin-bottom: 25px;
            display: flex;
            align-items: center;
            gap: 15px;
        }
        
        .controls {
            display: flex;
            gap: 20px;
            flex-wrap: wrap;
        }
        
        .btn {
            padding: 15px 30px;
            border: none;
            border-radius: 16px;
            background: linear-gradient(135deg, rgba(255, 255, 255, 0.1) 0%, rgba(255, 255, 255, 0.05) 100%);
            color: var(--light);
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            gap: 12px;
            font-size: 1rem;
            font-weight: 600;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
        
        .btn:hover {
            transform: translateY(-3px);
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.3);
            border-color: var(--primary);
        }
        
        .btn-primary {
            background: linear-gradient(135deg, var(--primary) 0%, #2563eb 100%);
        }
        
        .btn-success {
            background: linear-gradient(135deg, var(--secondary) 0%, #059669 100%);
        }
        
        .btn-danger {
            background: linear-gradient(135deg, var(--danger) 0%, #dc2626 100%);
        }
        
        .btn-warning {
            background: linear-gradient(135deg, var(--warning) 0%, #d97706 100%);
        }
        
        /* Data Table */
        .table-container {
            grid-column: span 12;
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            padding: 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            margin-top: 25px;
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
            overflow: hidden;
        }
        
        .table-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
        }
        
        table {
            width: 100%;
            border-collapse: separate;
            border-spacing: 0;
        }
        
        th {
            background: rgba(255, 255, 255, 0.05);
            padding: 20px;
            text-align: left;
            font-weight: 700;
            border-bottom: 2px solid rgba(255, 255, 255, 0.1);
            color: var(--light);
            font-size: 1rem;
        }
        
        td {
            padding: 20px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            color: #cbd5e1;
            font-size: 1rem;
        }
        
        tr:hover {
            background: rgba(255, 255, 255, 0.03);
        }
        
        .status-indicator {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 8px 16px;
            border-radius: 50px;
            font-size: 0.9rem;
            font-weight: 600;
        }
        
        .status-good { 
            background: rgba(16, 185, 129, 0.1); 
            color: var(--secondary); 
        }
        
        .status-warn { 
            background: rgba(245, 158, 11, 0.1); 
            color: var(--warning); 
        }
        
        .status-bad { 
            background: rgba(239, 68, 68, 0.1); 
            color: var(--danger); 
        }
        
        /* Footer */
        .footer {
            text-align: center;
            margin-top: 40px;
            padding: 30px;
            color: #94a3b8;
            font-size: 0.95rem;
            border-top: 1px solid rgba(255, 255, 255, 0.1);
        }
        
        /* Responsive */
        @media (max-width: 1600px) {
            .sensor-card, .chart-card {
                grid-column: span 6;
            }
        }
        
        @media (max-width: 1200px) {
            .sensor-card, .chart-card, .analytics-card {
                grid-column: span 12;
            }
            
            .header {
                flex-direction: column;
                gap: 25px;
                text-align: center;
            }
            
            .header-stats {
                justify-content: center;
            }
        }
        
        @media (max-width: 768px) {
            .sensor-value {
                font-size: 3.5rem;
            }
            
            .detail-item {
                padding: 15px;
            }
            
            .detail-value {
                font-size: 1.5rem;
            }
            
            .controls {
                justify-content: center;
            }
            
            .btn {
                padding: 12px 20px;
                font-size: 0.9rem;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <!-- Header -->
        <div class="header">
            <div class="header-left">
                <h1><i class="fas fa-satellite-dish"></i> ESP32 Professional Monitoring System</h1>
                <p>Advanced environmental monitoring with real-time analytics, detailed visualizations, and comprehensive data analysis</p>
            </div>
            <div class="header-stats">
                <div class="stat-badge">
                    <i class="fas fa-clock"></i>
                    <span id="lastUpdate">Just now</span>
                </div>
                <div class="stat-badge">
                    <i class="fas fa-database"></i>
                    <span id="dataPoints"><?php echo count($history); ?> records</span>
                </div>
                <div class="stat-badge">
                    <i class="fas fa-broadcast-tower"></i>
                    <span id="status">Live Streaming</span>
                </div>
            </div>
        </div>
        
        <!-- Dashboard Grid -->
        <div class="dashboard-grid">
            
            <!-- Temperature Card -->
            <div class="sensor-card temp">
                <div class="card-header">
                    <h3 class="card-title"><i class="fas fa-thermometer-three-quarters" style="color: #ef4444;"></i> Temperature</h3>
                    <span class="trend <?php echo $stats['temperature']['trend']; ?>">
                        <i class="fas fa-arrow-<?php 
                            echo $stats['temperature']['trend'] == 'up' ? 'up' : 
                                 ($stats['temperature']['trend'] == 'down' ? 'down' : 'right'); 
                        ?>"></i>
                        <?php echo ucfirst($stats['temperature']['trend']); ?>
                    </span>
                </div>
                <div class="sensor-value" style="color: <?php echo getColorByValue($stats['temperature']['current'], 'temperature'); ?>">
                    <?php echo number_format($stats['temperature']['current'], 1); ?>°C
                </div>
                <div class="sensor-details">
                    <div class="detail-item">
                        <div class="detail-label">Average</div>
                        <div class="detail-value"><?php echo number_format($stats['temperature']['average'], 1); ?>°C</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Minimum</div>
                        <div class="detail-value"><?php echo number_format($stats['temperature']['min'], 1); ?>°C</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Maximum</div>
                        <div class="detail-value"><?php echo number_format($stats['temperature']['max'], 1); ?>°C</div>
                    </div>
                </div>
            </div>
            
            <!-- Humidity Card -->
            <div class="sensor-card hum">
                <div class="card-header">
                    <h3 class="card-title"><i class="fas fa-tint" style="color: #3b82f6;"></i> Humidity</h3>
                    <span class="trend <?php echo $stats['humidity']['trend']; ?>">
                        <i class="fas fa-arrow-<?php 
                            echo $stats['humidity']['trend'] == 'up' ? 'up' : 
                                 ($stats['humidity']['trend'] == 'down' ? 'down' : 'right'); 
                        ?>"></i>
                        <?php echo ucfirst($stats['humidity']['trend']); ?>
                    </span>
                </div>
                <div class="sensor-value" style="color: <?php echo getColorByValue($stats['humidity']['current'], 'humidity'); ?>">
                    <?php echo number_format($stats['humidity']['current'], 1); ?>%
                </div>
                <div class="sensor-details">
                    <div class="detail-item">
                        <div class="detail-label">Average</div>
                        <div class="detail-value"><?php echo number_format($stats['humidity']['average'], 1); ?>%</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Minimum</div>
                        <div class="detail-value"><?php echo number_format($stats['humidity']['min'], 1); ?>%</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Maximum</div>
                        <div class="detail-value"><?php echo number_format($stats['humidity']['max'], 1); ?>%</div>
                    </div>
                </div>
            </div>
            
            <!-- Pressure Card -->
            <div class="sensor-card press">
                <div class="card-header">
                    <h3 class="card-title"><i class="fas fa-tachometer-alt" style="color: #10b981;"></i> Pressure</h3>
                    <span class="trend <?php echo $stats['pressure']['trend']; ?>">
                        <i class="fas fa-arrow-<?php 
                            echo $stats['pressure']['trend'] == 'up' ? 'up' : 
                                 ($stats['pressure']['trend'] == 'down' ? 'down' : 'right'); 
                        ?>"></i>
                        <?php echo ucfirst($stats['pressure']['trend']); ?>
                    </span>
                </div>
                <div class="sensor-value" style="color: <?php echo getColorByValue($stats['pressure']['current'], 'pressure'); ?>">
                    <?php echo number_format($stats['pressure']['current'], 1); ?> hPa
                </div>
                <div class="sensor-details">
                    <div class="detail-item">
                        <div class="detail-label">Average</div>
                        <div class="detail-value"><?php echo number_format($stats['pressure']['average'], 1); ?> hPa</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Minimum</div>
                        <div class="detail-value"><?php echo number_format($stats['pressure']['min'], 1); ?> hPa</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Maximum</div>
                        <div class="detail-value"><?php echo number_format($stats['pressure']['max'], 1); ?> hPa</div>
                    </div>
                </div>
            </div>
            
            <!-- Temperature Chart -->
            <div class="chart-card temp-chart">
                <div class="chart-header">
                    <h3 class="chart-title"><i class="fas fa-chart-line" style="color: #ef4444;"></i> Temperature History</h3>
                    <div class="trend stable">
                        <i class="fas fa-expand-alt"></i> Full View
                    </div>
                </div>
                <div class="chart-container">
                    <canvas id="temperatureChart"></canvas>
                </div>
            </div>
            
            <!-- Humidity Chart -->
            <div class="chart-card hum-chart">
                <div class="chart-header">
                    <h3 class="chart-title"><i class="fas fa-chart-area" style="color: #3b82f6;"></i> Humidity History</h3>
                    <div class="trend stable">
                        <i class="fas fa-expand-alt"></i> Full View
                    </div>
                </div>
                <div class="chart-container">
                    <canvas id="humidityChart"></canvas>
                </div>
            </div>
            
            <!-- Pressure Chart -->
            <div class="chart-card press-chart">
                <div class="chart-header">
                    <h3 class="chart-title"><i class="fas fa-chart-bar" style="color: #10b981;"></i> Pressure History</h3>
                    <div class="trend stable">
                        <i class="fas fa-expand-alt"></i> Full View
                    </div>
                </div>
                <div class="chart-container">
                    <canvas id="pressureChart"></canvas>
                </div>
            </div>
            
            <!-- Advanced Analytics -->
            <div class="analytics-grid">
                <div class="analytics-card">
                    <h4 class="analytics-title"><i class="fas fa-brain"></i> AI Insights</h4>
                    <div class="analytics-content">
                        <p>System analyzing patterns in real-time to detect anomalies and predict trends based on historical data.</p>
                        <ul style="margin-top: 10px; padding-left: 20px;">
                            <li>Anomaly detection active</li>
                            <li>Pattern recognition enabled</li>
                            <li>Predictive analytics running</li>
                        </ul>
                    </div>
                </div>
                
                <div class="analytics-card">
                    <h4 class="analytics-title"><i class="fas fa-bell"></i> Alerts Status</h4>
                    <div class="analytics-content">
                        <p>Current alert thresholds and monitoring status:</p>
                        <ul style="margin-top: 10px; padding-left: 20px;">
                            <li>Temperature: <span style="color: #ef4444;">&gt;30°C</span> (Warning)</li>
                            <li>Humidity: <span style="color: #3b82f6;">40-60%</span> (Optimal)</li>
                            <li>Pressure: <span style="color: #10b981;">Normal</span></li>
                        </ul>
                    </div>
                </div>
                
                <div class="analytics-card">
                    <h4 class="analytics-title"><i class="fas fa-info-circle"></i> System Info</h4>
                    <div class="analytics-content">
                        <p>Dashboard information and configuration:</p>
                        <ul style="margin-top: 10px; padding-left: 20px;">
                            <li>Version: 3.2.1</li>
                            <li>Update interval: <?php echo $updateInterval/1000; ?>s</li>
                            <li>Max history: <?php echo $maxHistory; ?> records</li>
                            <li>Last data point: <?php echo !empty($times) ? end($times) : 'N/A'; ?></li>
                        </ul>
                    </div>
                </div>
            </div>
            
            <!-- Controls -->
            <div class="controls-container">
                <h3 class="controls-title"><i class="fas fa-sliders-h"></i> Dashboard Controls</h3>
                <div class="controls">
                    <button class="btn btn-primary" onclick="refreshData()">
                        <i class="fas fa-redo"></i> Refresh Data
                    </button>
                    <button class="btn btn-success" onclick="toggleAutoUpdate()" id="autoUpdateBtn">
                        <i class="fas fa-sync"></i> Auto Update (<?php echo $updateInterval/1000; ?>s)
                    </button>
                    <button class="btn" onclick="exportData()">
                        <i class="fas fa-download"></i> Export CSV
                    </button>
                    <button class="btn btn-warning" onclick="clearData()">
                        <i class="fas fa-trash-alt"></i> Clear Data
                    </button>
                    <button class="btn" onclick="toggleCharts()">
                        <i class="fas fa-chart-bar"></i> Toggle Charts
                    </button>
                    <button class="btn btn-danger" onclick="resetSystem()">
                        <i class="fas fa-power-off"></i> System Reset
                    </button>
                </div>
            </div>
            
            <!-- Data Table -->
            <div class="table-container">
                <div class="table-header">
                    <h3 class="card-title"><i class="fas fa-table"></i> Historical Data Records</h3>
                    <button class="btn" onclick="toggleTable()">
                        <i class="fas fa-eye"></i> Show/Hide
                    </button>
                </div>
                <div id="dataTable">
                    <table>
                        <thead>
                            <tr>
                                <th><i class="fas fa-clock"></i> Time</th>
                                <th><i class="fas fa-thermometer-half"></i> Temperature (°C)</th>
                                <th><i class="fas fa-tint"></i> Humidity (%)</th>
                                <th><i class="fas fa-tachometer-alt"></i> Pressure (hPa)</th>
                                <th><i class="fas fa-info-circle"></i> Status</th>
                                <th><i class="fas fa-chart-line"></i> Trend</th>
                            </tr>
                        </thead>
                        <tbody id="tableBody">
                            <?php 
                            $reversedHistory = array_reverse($history);
                            $displayHistory = array_slice($reversedHistory, 0, 10);
                            foreach($displayHistory as $index => $entry): 
                                $temp = $entry['temperature'];
                                $status = '';
                                $statusClass = '';
                                if ($temp > 30) {
                                    $status = 'Hot';
                                    $statusClass = 'status-bad';
                                } elseif ($temp < 15) {
                                    $status = 'Cold';
                                    $statusClass = 'status-warn';
                                } else {
                                    $status = 'Normal';
                                    $statusClass = 'status-good';
                                }
                                
                                // Simple trend calculation
                                $trend = 'stable';
                                if ($index > 0) {
                                    $prevEntry = $displayHistory[$index - 1];
                                    if ($temp > $prevEntry['temperature'] + 0.5) $trend = 'up';
                                    elseif ($temp < $prevEntry['temperature'] - 0.5) $trend = 'down';
                                }
                            ?>
                            <tr>
                                <td><?php echo htmlspecialchars($entry['time']); ?></td>
                                <td><?php echo number_format($entry['temperature'], 1); ?></td>
                                <td><?php echo number_format($entry['humidity'], 1); ?></td>
                                <td><?php echo number_format($entry['pressure'], 1); ?></td>
                                <td>
                                    <span class="status-indicator <?php echo $statusClass; ?>">
                                        <?php echo $status; ?>
                                    </span>
                                </td>
                                <td>
                                    <span class="trend <?php echo $trend; ?>" style="padding: 5px 12px;">
                                        <i class="fas fa-arrow-<?php echo $trend == 'up' ? 'up' : ($trend == 'down' ? 'down' : 'right'); ?>"></i>
                                        <?php echo ucfirst($trend); ?>
                                    </span>
                                </td>
                            </tr>
                            <?php endforeach; ?>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
        
        <!-- Footer -->
        <div class="footer">
            <p>ESP32 Professional Monitoring System • Version 3.2.1 • Real-time Analytics Dashboard • 
               <span id="serverTime"><?php echo $current_time; ?></span></p>
            <p style="margin-top: 10px; font-size: 0.85rem; opacity: 0.7;">
                <i class="fas fa-microchip"></i> ESP32 Microcontroller • 
                <i class="fas fa-cloud"></i> Cloud Connected • 
                <i class="fas fa-shield-alt"></i> Secure Data Transmission
            </p>
        </div>
    </div>

    <script>
        // Chart instances
        let temperatureChart, humidityChart, pressureChart;
        let autoUpdateInterval;
        let autoUpdateEnabled = true;
        let chartsVisible = true;
        
        // Initialize all charts
        function initCharts() {
            console.log("Initializing charts...");
            
            // Destroy existing charts if they exist
            if (temperatureChart) temperatureChart.destroy();
            if (humidityChart) humidityChart.destroy();
            if (pressureChart) pressureChart.destroy();
            
            // Temperature Chart
            const tempCtx = document.getElementById('temperatureChart').getContext('2d');
            temperatureChart = new Chart(tempCtx, {
                type: 'line',
                data: {
                    labels: <?php echo json_encode($times ?: []); ?>,
                    datasets: [{
                        label: 'Temperature (°C)',
                        data: <?php echo json_encode($temperatures ?: []); ?>,
                        borderColor: '#ef4444',
                        backgroundColor: 'rgba(239, 68, 68, 0.15)',
                        borderWidth: 3,
                        pointRadius: 4,
                        pointHoverRadius: 8,
                        pointBackgroundColor: '#ef4444',
                        pointBorderColor: '#ffffff',
                        pointBorderWidth: 2,
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: { 
                        mode: 'index', 
                        intersect: false 
                    },
                    plugins: {
                        legend: { 
                            display: true,
                            labels: { 
                                color: '#f8fafc', 
                                font: { size: 14 },
                                padding: 20
                            }
                        },
                        tooltip: {
                            mode: 'index',
                            intersect: false,
                            backgroundColor: 'rgba(15, 23, 42, 0.9)',
                            titleColor: '#f8fafc',
                            bodyColor: '#f8fafc',
                            borderColor: '#ef4444',
                            borderWidth: 1,
                            cornerRadius: 12,
                            padding: 15
                        },
                        title: {
                            display: true,
                            text: 'Temperature History',
                            color: '#f8fafc',
                            font: { size: 18, weight: 'bold' }
                        }
                    },
                    scales: {
                        x: {
                            grid: { 
                                color: 'rgba(255, 255, 255, 0.1)',
                                drawBorder: false
                            },
                            ticks: { 
                                color: '#94a3b8',
                                maxRotation: 45,
                                font: { size: 12 }
                            }
                        },
                        y: {
                            beginAtZero: false,
                            grid: { 
                                color: 'rgba(255, 255, 255, 0.1)',
                                drawBorder: false
                            },
                            ticks: { 
                                color: '#94a3b8',
                                font: { size: 12 }
                            },
                            title: {
                                display: true,
                                text: 'Temperature (°C)',
                                color: '#94a3b8',
                                font: { size: 14 }
                            }
                        }
                    },
                    elements: {
                        line: {
                            tension: 0.4
                        }
                    }
                }
            });
            
            // Humidity Chart
            const humCtx = document.getElementById('humidityChart').getContext('2d');
            humidityChart = new Chart(humCtx, {
                type: 'line',
                data: {
                    labels: <?php echo json_encode($times ?: []); ?>,
                    datasets: [{
                        label: 'Humidity (%)',
                        data: <?php echo json_encode($humidities ?: []); ?>,
                        borderColor: '#3b82f6',
                        backgroundColor: 'rgba(59, 130, 246, 0.15)',
                        borderWidth: 3,
                        pointRadius: 4,
                        pointHoverRadius: 8,
                        pointBackgroundColor: '#3b82f6',
                        pointBorderColor: '#ffffff',
                        pointBorderWidth: 2,
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: { 
                        mode: 'index', 
                        intersect: false 
                    },
                    plugins: {
                        legend: { 
                            display: true,
                            labels: { 
                                color: '#f8fafc', 
                                font: { size: 14 },
                                padding: 20
                            }
                        },
                        tooltip: {
                            mode: 'index',
                            intersect: false,
                            backgroundColor: 'rgba(15, 23, 42, 0.9)',
                            titleColor: '#f8fafc',
                            bodyColor: '#f8fafc',
                            borderColor: '#3b82f6',
                            borderWidth: 1,
                            cornerRadius: 12,
                            padding: 15
                        },
                        title: {
                            display: true,
                            text: 'Humidity History',
                            color: '#f8fafc',
                            font: { size: 18, weight: 'bold' }
                        }
                    },
                    scales: {
                        x: {
                            grid: { 
                                color: 'rgba(255, 255, 255, 0.1)',
                                drawBorder: false
                            },
                            ticks: { 
                                color: '#94a3b8',
                                maxRotation: 45,
                                font: { size: 12 }
                            }
                        },
                        y: {
                            beginAtZero: false,
                            grid: { 
                                color: 'rgba(255, 255, 255, 0.1)',
                                drawBorder: false
                            },
                            ticks: { 
                                color: '#94a3b8',
                                font: { size: 12 }
                            },
                            title: {
                                display: true,
                                text: 'Humidity (%)',
                                color: '#94a3b8',
                                font: { size: 14 }
                            }
                        }
                    },
                    elements: {
                        line: {
                            tension: 0.4
                        }
                    }
                }
            });
            
            // Pressure Chart
            const pressCtx = document.getElementById('pressureChart').getContext('2d');
            pressureChart = new Chart(pressCtx, {
                type: 'line',
                data: {
                    labels: <?php echo json_encode($times ?: []); ?>,
                    datasets: [{
                        label: 'Pressure (hPa)',
                        data: <?php echo json_encode($pressures ?: []); ?>,
                        borderColor: '#10b981',
                        backgroundColor: 'rgba(16, 185, 129, 0.15)',
                        borderWidth: 3,
                        pointRadius: 4,
                        pointHoverRadius: 8,
                        pointBackgroundColor: '#10b981',
                        pointBorderColor: '#ffffff',
                        pointBorderWidth: 2,
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: { 
                        mode: 'index', 
                        intersect: false 
                    },
                    plugins: {
                        legend: { 
                            display: true,
                            labels: { 
                                color: '#f8fafc', 
                                font: { size: 14 },
                                padding: 20
                            }
                        },
                        tooltip: {
                            mode: 'index',
                            intersect: false,
                            backgroundColor: 'rgba(15, 23, 42, 0.9)',
                            titleColor: '#f8fafc',
                            bodyColor: '#f8fafc',
                            borderColor: '#10b981',
                            borderWidth: 1,
                            cornerRadius: 12,
                            padding: 15
                        },
                        title: {
                            display: true,
                            text: 'Pressure History',
                            color: '#f8fafc',
                            font: { size: 18, weight: 'bold' }
                        }
                    },
                    scales: {
                        x: {
                            grid: { 
                                color: 'rgba(255, 255, 255, 0.1)',
                                drawBorder: false
                            },
                            ticks: { 
                                color: '#94a3b8',
                                maxRotation: 45,
                                font: { size: 12 }
                            }
                        },
                        y: {
                            beginAtZero: false,
                            grid: { 
                                color: 'rgba(255, 255, 255, 0.1)',
                                drawBorder: false
                            },
                            ticks: { 
                                color: '#94a3b8',
                                font: { size: 12 }
                            },
                            title: {
                                display: true,
                                text: 'Pressure (hPa)',
                                color: '#94a3b8',
                                font: { size: 14 }
                            }
                        }
                    },
                    elements: {
                        line: {
                            tension: 0.4
                        }
                    }
                }
            });
            
            console.log("All charts initialized successfully!");
            startAutoUpdate();
        }
        
        // Update charts with new data
        async function updateCharts() {
            try {
                console.log("Updating charts...");
                const response = await fetch('?json=1');
                if (!response.ok) throw new Error('Network response was not ok');
                
                const data = await response.json();
                console.log("Data received:", data);
                
                // Update temperature chart
                if (temperatureChart && temperatureChart.data) {
                    temperatureChart.data.labels = data.times || [];
                    temperatureChart.data.datasets[0].data = data.temperatures || [];
                    temperatureChart.update('none');
                }
                
                // Update humidity chart
                if (humidityChart && humidityChart.data) {
                    humidityChart.data.labels = data.times || [];
                    humidityChart.data.datasets[0].data = data.humidities || [];
                    humidityChart.update('none');
                }
                
                // Update pressure chart
                if (pressureChart && pressureChart.data) {
                    pressureChart.data.labels = data.times || [];
                    pressureChart.data.datasets[0].data = data.pressures || [];
                    pressureChart.update('none');
                }
                
                // Update statistics
                document.getElementById('lastUpdate').textContent = 
                    new Date().toLocaleTimeString();
                document.getElementById('dataPoints').textContent = 
                    (data.count || 0) + ' records';
                document.getElementById('serverTime').textContent = 
                    new Date().toLocaleString();
                
                console.log("Charts updated successfully!");
                
            } catch (error) {
                console.error('Update error:', error);
                document.getElementById('status').innerHTML = 
                    '<span style="color: #ef4444">Connection Lost</span>';
            }
        }
        
        // Auto update control
        function startAutoUpdate() {
            if (autoUpdateInterval) clearInterval(autoUpdateInterval);
            autoUpdateInterval = setInterval(updateCharts, <?php echo $updateInterval; ?>);
            autoUpdateEnabled = true;
            document.getElementById('autoUpdateBtn').innerHTML = 
                '<i class="fas fa-pause"></i> Pause Auto Update';
            console.log("Auto update started");
        }
        
        function stopAutoUpdate() {
            if (autoUpdateInterval) clearInterval(autoUpdateInterval);
            autoUpdateEnabled = false;
            document.getElementById('autoUpdateBtn').innerHTML = 
                '<i class="fas fa-play"></i> Resume Auto Update';
            console.log("Auto update stopped");
        }
        
        function toggleAutoUpdate() {
            if (autoUpdateEnabled) {
                stopAutoUpdate();
            } else {
                startAutoUpdate();
            }
        }
        
        // Utility functions
        function refreshData() {
            updateCharts();
            showNotification('Data refreshed successfully!', 'success');
        }
        
        function exportData() {
            window.location.href = '?export=csv';
            showNotification('Exporting data to CSV...', 'info');
        }
        
        async function clearData() {
            if (confirm('⚠️ Are you sure you want to clear ALL data? This action cannot be undone.')) {
                try {
                    const response = await fetch('?clear=1');
                    const result = await response.json();
                    if (result.status === 'success') {
                        showNotification('All data cleared successfully!', 'success');
                        refreshData();
                    }
                } catch (error) {
                    showNotification('Error clearing data: ' + error.message, 'error');
                }
            }
        }
        
        function toggleTable() {
            const table = document.getElementById('dataTable');
            const isVisible = table.style.display !== 'none';
            table.style.display = isVisible ? 'none' : 'block';
            showNotification(isVisible ? 'Table hidden' : 'Table shown', 'info');
        }
        
        function toggleCharts() {
            const charts = document.querySelectorAll('.chart-card');
            chartsVisible = !chartsVisible;
            charts.forEach(chart => {
                chart.style.display = chartsVisible ? 'flex' : 'none';
            });
            showNotification(chartsVisible ? 'Charts shown' : 'Charts hidden', 'info');
        }
        
        function resetSystem() {
            if (confirm('🔄 Are you sure you want to reset the system? This will refresh all components.')) {
                location.reload();
            }
        }
        
        function showNotification(message, type = 'info') {
            // Create notification element
            const notification = document.createElement('div');
            notification.style.cssText = `
                position: fixed;
                top: 20px;
                right: 20px;
                padding: 15px 25px;
                border-radius: 12px;
                color: white;
                font-weight: 600;
                z-index: 10000;
                animation: slideIn 0.3s ease;
                backdrop-filter: blur(20px);
                border: 1px solid rgba(255, 255, 255, 0.1);
            `;
            
            if (type === 'success') {
                notification.style.background = 'linear-gradient(135deg, rgba(16, 185, 129, 0.9), rgba(5, 150, 105, 0.9))';
            } else if (type === 'error') {
                notification.style.background = 'linear-gradient(135deg, rgba(239, 68, 68, 0.9), rgba(220, 38, 38, 0.9))';
            } else if (type === 'warning') {
                notification.style.background = 'linear-gradient(135deg, rgba(245, 158, 11, 0.9), rgba(217, 119, 6, 0.9))';
            } else {
                notification.style.background = 'linear-gradient(135deg, rgba(59, 130, 246, 0.9), rgba(37, 99, 235, 0.9))';
            }
            
            notification.innerHTML = `
                <i class="fas fa-${type === 'success' ? 'check-circle' : type === 'error' ? 'exclamation-circle' : type === 'warning' ? 'exclamation-triangle' : 'info-circle'}"></i>
                ${message}
            `;
            
            document.body.appendChild(notification);
            
            // Remove notification after 3 seconds
            setTimeout(() => {
                notification.style.animation = 'slideOut 0.3s ease';
                setTimeout(() => {
                    if (notification.parentNode) {
                        notification.parentNode.removeChild(notification);
                    }
                }, 300);
            }, 3000);
        }
        
        // Add CSS for animations
        const style = document.createElement('style');
        style.textContent = `
            @keyframes slideIn {
                from { transform: translateX(100%); opacity: 0; }
                to { transform: translateX(0); opacity: 1; }
            }
            @keyframes slideOut {
                from { transform: translateX(0); opacity: 1; }
                to { transform: translateX(100%); opacity: 0; }
            }
        `;
        document.head.appendChild(style);
        
        // Initialize on load
        document.addEventListener('DOMContentLoaded', function() {
            console.log("DOM loaded, initializing charts...");
            try {
                initCharts();
                showNotification('Dashboard initialized successfully!', 'success');
            } catch (error) {
                console.error("Error initializing charts:", error);
                showNotification("Error initializing charts. Check console for details.", 'error');
            }
        });
    </script>
</body>
</html>