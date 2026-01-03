const express = require("express");
const app = express();

app.use(express.json());

const MAX_HISTORY = 10000;

let history = [];

// ================= API =================

app.post("/api/data", (req, res) => {
  const { temperature, pressure, humidity } = req.body;

  if (
    typeof temperature !== "number" ||
    typeof pressure !== "number" ||
    typeof humidity !== "number"
  ) {
    return res.status(400).json({ error: "Invalid JSON payload" });
  }

  const entry = {
    temperature: parseFloat(temperature.toFixed(1)),
    pressure: parseFloat(pressure.toFixed(1)),
    humidity: parseFloat(humidity.toFixed(1)),
    time: new Date().toISOString(),
    timestamp: Date.now()
  };

  history.push(entry);

  if (history.length > MAX_HISTORY) {
    history.shift();
  }

  console.log("Received:", entry);
  res.sendStatus(200);
});

app.get("/api/history", (req, res) => {
  res.json(history);
});

app.get("/api/stats", (req, res) => {
  if (history.length === 0) {
    return res.json({
      temperature: { current: 0, average: 0, min: 0, max: 0, trend: 'stable' },
      humidity: { current: 0, average: 0, min: 0, max: 0, trend: 'stable' },
      pressure: { current: 0, average: 0, min: 0, max: 0, trend: 'stable' },
      count: 0,
      lastUpdate: new Date().toISOString()
    });
  }

  const temps = history.map(h => h.temperature);
  const hums = history.map(h => h.humidity);
  const press = history.map(h => h.pressure);

  // Calculate trends based on last 10 points
  const getTrend = (data) => {
    if (data.length < 5) return 'stable';
    const recent = data.slice(-5);
    const first = recent[0];
    const last = recent[recent.length - 1];
    const diff = last - first;
    
    if (diff > 1) return 'up';
    if (diff < -1) return 'down';
    return 'stable';
  };

  res.json({
    temperature: {
      current: temps[temps.length - 1],
      average: parseFloat((temps.reduce((a, b) => a + b, 0) / temps.length).toFixed(1)),
      min: Math.min(...temps),
      max: Math.max(...temps),
      trend: getTrend(temps)
    },
    humidity: {
      current: hums[hums.length - 1],
      average: parseFloat((hums.reduce((a, b) => a + b, 0) / hums.length).toFixed(1)),
      min: Math.min(...hums),
      max: Math.max(...hums),
      trend: getTrend(hums)
    },
    pressure: {
      current: press[press.length - 1],
      average: parseFloat((press.reduce((a, b) => a + b, 0) / press.length).toFixed(1)),
      min: Math.min(...press),
      max: Math.max(...press),
      trend: getTrend(press)
    },
    count: history.length,
    lastUpdate: history[history.length - 1].time
  });
});

app.get("/api/export", (req, res) => {
  const csv = [
    "Time,Temperature (°C),Humidity (%),Pressure (hPa)",
    ...history.map(h => 
      `${new Date(h.time).toLocaleString()},${h.temperature},${h.humidity},${h.pressure}`
    )
  ].join("\n");

  res.header('Content-Type', 'text/csv');
  res.attachment('sensor_data.csv');
  res.send(csv);
});

app.post("/api/clear", (req, res) => {
  history = [];
  res.json({ status: "success", message: "Data cleared" });
});

// ================= FRONTEND =================

app.get("/", (req, res) => {
  res.send(`
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🌡️ ESP32 Professional Monitoring Dashboard</title>
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
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', sans-serif;
            background: linear-gradient(135deg, var(--darker) 0%, var(--dark) 100%);
            color: var(--light);
            min-height: 100vh;
            overflow-x: hidden;
        }
        
        .container {
            max-width: 2000px;
            margin: 0 auto;
            padding: 20px;
        }
        
        /* Header Styles */
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
            animation: slideDown 0.8s ease-out;
        }
        
        @keyframes slideDown {
            from { transform: translateY(-50px); opacity: 0; }
            to { transform: translateY(0); opacity: 1; }
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
            transition: all 0.3s ease;
        }
        
        .stat-badge:hover {
            transform: translateY(-3px);
            background: rgba(255, 255, 255, 0.12);
            border-color: var(--primary);
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
            animation: fadeIn 0.6s ease-out 0.2s both;
        }
        
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(30px); }
            to { opacity: 1; transform: translateY(0); }
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
            background: linear-gradient(135deg, var(--light) 30%, #cbd5e1 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
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
            transition: all 0.3s ease;
        }
        
        .detail-item:hover {
            background: rgba(255, 255, 255, 0.08);
            border-color: rgba(255, 255, 255, 0.1);
            transform: translateY(-3px);
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
        
        /* Chart Cards */
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
            animation: fadeIn 0.6s ease-out 0.4s both;
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
        
        /* Time Display */
        .time-display {
            grid-column: span 12;
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.95) 0%, rgba(15, 23, 42, 0.95) 100%);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            padding: 25px 30px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            margin: 25px 0;
            box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
            display: flex;
            justify-content: space-between;
            align-items: center;
            animation: fadeIn 0.6s ease-out 0.6s both;
        }
        
        .current-time {
            font-size: 2.2rem;
            font-weight: 700;
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #60a5fa, #8b5cf6);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        
        .time-details {
            display: flex;
            gap: 30px;
        }
        
        .time-item {
            text-align: center;
        }
        
        .time-label {
            font-size: 0.9rem;
            color: #94a3b8;
            text-transform: uppercase;
            letter-spacing: 1px;
            margin-bottom: 8px;
        }
        
        .time-value {
            font-size: 1.4rem;
            font-weight: 600;
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
            animation: fadeIn 0.6s ease-out 0.8s both;
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
            position: sticky;
            top: 0;
        }
        
        td {
            padding: 20px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            color: #cbd5e1;
            font-size: 1rem;
        }
        
        tr {
            transition: all 0.3s ease;
        }
        
        tr:hover {
            background: rgba(255, 255, 255, 0.03);
            transform: translateX(5px);
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
        
        /* Footer */
        .footer {
            text-align: center;
            margin-top: 40px;
            padding: 30px;
            color: #94a3b8;
            font-size: 0.95rem;
            border-top: 1px solid rgba(255, 255, 255, 0.1);
        }
        
        .live-indicator {
            display: inline-flex;
            align-items: center;
            gap: 10px;
            margin-top: 15px;
        }
        
        .pulse {
            width: 12px;
            height: 12px;
            background: var(--secondary);
            border-radius: 50%;
            animation: pulse 2s infinite;
        }
        
        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7); }
            70% { box-shadow: 0 0 0 10px rgba(16, 185, 129, 0); }
            100% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0); }
        }
        
        /* Responsive */
        @media (max-width: 1600px) {
            .sensor-card, .chart-card {
                grid-column: span 6;
            }
        }
        
        @media (max-width: 1200px) {
            .sensor-card, .chart-card {
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
            
            .time-display {
                flex-direction: column;
                gap: 20px;
                text-align: center;
            }
            
            .time-details {
                width: 100%;
                justify-content: space-around;
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
            
            .current-time {
                font-size: 1.8rem;
            }
        }
        
        /* Loading Animation */
        .loading {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 200px;
        }
        
        .spinner {
            width: 50px;
            height: 50px;
            border: 3px solid rgba(255, 255, 255, 0.1);
            border-radius: 50%;
            border-top-color: var(--primary);
            animation: spin 1s ease-in-out infinite;
        }
        
        @keyframes spin {
            to { transform: rotate(360deg); }
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
                    <span id="lastUpdate">Loading...</span>
                </div>
                <div class="stat-badge">
                    <i class="fas fa-database"></i>
                    <span id="dataPoints">0 records</span>
                </div>
                <div class="stat-badge">
                    <i class="fas fa-broadcast-tower"></i>
                    <span id="status">Connecting...</span>
                </div>
            </div>
        </div>
        
        <!-- Dashboard Grid -->
        <div class="dashboard-grid">
            <!-- Temperature Card -->
            <div class="sensor-card temp">
                <div class="card-header">
                    <h3 class="card-title"><i class="fas fa-thermometer-three-quarters" style="color: #ef4444;"></i> Temperature</h3>
                    <span class="trend stable" id="tempTrend">
                        <i class="fas fa-arrow-right"></i>
                        Stable
                    </span>
                </div>
                <div class="sensor-value" id="tempCurrent" style="color: #ef4444;">
                    0.0°C
                </div>
                <div class="sensor-details">
                    <div class="detail-item">
                        <div class="detail-label">Average</div>
                        <div class="detail-value" id="tempAvg">0.0°C</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Minimum</div>
                        <div class="detail-value" id="tempMin">0.0°C</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Maximum</div>
                        <div class="detail-value" id="tempMax">0.0°C</div>
                    </div>
                </div>
            </div>
            
            <!-- Humidity Card -->
            <div class="sensor-card hum">
                <div class="card-header">
                    <h3 class="card-title"><i class="fas fa-tint" style="color: #3b82f6;"></i> Humidity</h3>
                    <span class="trend stable" id="humTrend">
                        <i class="fas fa-arrow-right"></i>
                        Stable
                    </span>
                </div>
                <div class="sensor-value" id="humCurrent" style="color: #3b82f6;">
                    0.0%
                </div>
                <div class="sensor-details">
                    <div class="detail-item">
                        <div class="detail-label">Average</div>
                        <div class="detail-value" id="humAvg">0.0%</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Minimum</div>
                        <div class="detail-value" id="humMin">0.0%</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Maximum</div>
                        <div class="detail-value" id="humMax">0.0%</div>
                    </div>
                </div>
            </div>
            
            <!-- Pressure Card -->
            <div class="sensor-card press">
                <div class="card-header">
                    <h3 class="card-title"><i class="fas fa-tachometer-alt" style="color: #10b981;"></i> Pressure</h3>
                    <span class="trend stable" id="pressTrend">
                        <i class="fas fa-arrow-right"></i>
                        Stable
                    </span>
                </div>
                <div class="sensor-value" id="pressCurrent" style="color: #10b981;">
                    0.0 hPa
                </div>
                <div class="sensor-details">
                    <div class="detail-item">
                        <div class="detail-label">Average</div>
                        <div class="detail-value" id="pressAvg">0.0 hPa</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Minimum</div>
                        <div class="detail-value" id="pressMin">0.0 hPa</div>
                    </div>
                    <div class="detail-item">
                        <div class="detail-label">Maximum</div>
                        <div class="detail-value" id="pressMax">0.0 hPa</div>
                    </div>
                </div>
            </div>
            
            <!-- Time Display -->
            <div class="time-display">
                <div class="current-time" id="currentTime">
                    00:00:00
                </div>
                <div class="time-details">
                    <div class="time-item">
                        <div class="time-label">Date</div>
                        <div class="time-value" id="currentDate">Loading...</div>
                    </div>
                    <div class="time-item">
                        <div class="time-label">Uptime</div>
                        <div class="time-value" id="uptime">0h 0m 0s</div>
                    </div>
                    <div class="time-item">
                        <div class="time-label">Update Rate</div>
                        <div class="time-value">5s</div>
                    </div>
                </div>
            </div>
            
            <!-- Charts -->
            <div class="chart-card temp-chart">
                <div class="chart-header">
                    <h3 class="chart-title"><i class="fas fa-chart-line" style="color: #ef4444;"></i> Temperature History</h3>
                    <div class="trend stable">
                        <i class="fas fa-history"></i> Last 24h
                    </div>
                </div>
                <div class="chart-container">
                    <canvas id="temperatureChart"></canvas>
                </div>
            </div>
            
            <div class="chart-card hum-chart">
                <div class="chart-header">
                    <h3 class="chart-title"><i class="fas fa-chart-area" style="color: #3b82f6;"></i> Humidity History</h3>
                    <div class="trend stable">
                        <i class="fas fa-history"></i> Last 24h
                    </div>
                </div>
                <div class="chart-container">
                    <canvas id="humidityChart"></canvas>
                </div>
            </div>
            
            <div class="chart-card press-chart">
                <div class="chart-header">
                    <h3 class="chart-title"><i class="fas fa-chart-bar" style="color: #10b981;"></i> Pressure History</h3>
                    <div class="trend stable">
                        <i class="fas fa-history"></i> Last 24h
                    </div>
                </div>
                <div class="chart-container">
                    <canvas id="pressureChart"></canvas>
                </div>
            </div>
            
            <!-- Data Table -->
            <div class="table-container">
                <div class="table-header">
                    <h3 class="card-title"><i class="fas fa-table"></i> Historical Data Records</h3>
                    <div class="controls">
                        <button class="btn" onclick="refreshTable()">
                            <i class="fas fa-redo"></i> Refresh
                        </button>
                        <button class="btn" onclick="toggleTable()" id="tableToggle">
                            <i class="fas fa-eye"></i> Show All
                        </button>
                    </div>
                </div>
                <div class="table-wrapper">
                    <table>
                        <thead>
                            <tr>
                                <th><i class="fas fa-calendar-alt"></i> Date & Time</th>
                                <th><i class="fas fa-clock"></i> Time</th>
                                <th><i class="fas fa-thermometer-half"></i> Temp (°C)</th>
                                <th><i class="fas fa-tint"></i> Hum (%)</th>
                                <th><i class="fas fa-tachometer-alt"></i> Press (hPa)</th>
                                <th><i class="fas fa-info-circle"></i> Status</th>
                                <th><i class="fas fa-chart-line"></i> Trend</th>
                            </tr>
                        </thead>
                        <tbody id="tableBody">
                            <tr>
                                <td colspan="7" style="text-align: center; padding: 50px;">
                                    <div class="loading">
                                        <div class="spinner"></div>
                                    </div>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
            
            <!-- Controls -->
            <div class="controls-container">
                <h3 class="controls-title"><i class="fas fa-sliders-h"></i> Dashboard Controls</h3>
                <div class="controls">
                    <button class="btn btn-primary" onclick="refreshAllData()">
                        <i class="fas fa-redo"></i> Refresh All
                    </button>
                    <button class="btn btn-success" onclick="toggleAutoUpdate()" id="autoUpdateBtn">
                        <i class="fas fa-play"></i> Auto Update (5s)
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
                    <button class="btn btn-danger" onclick="resetDashboard()">
                        <i class="fas fa-power-off"></i> Reset Dashboard
                    </button>
                </div>
            </div>
        </div>
        
        <!-- Footer -->
        <div class="footer">
            <p>ESP32 Professional Monitoring System • Version 3.2.1 • Real-time Analytics Dashboard</p>
            <div class="live-indicator">
                <div class="pulse"></div>
                <span id="connectionStatus">Connected</span>
                <span id="serverTime">Server time: Loading...</span>
            </div>
        </div>
    </div>

    <script>
        // Chart instances
        let temperatureChart, humidityChart, pressureChart;
        let autoUpdateInterval = null;
        let autoUpdateEnabled = true;
        let startTime = Date.now();
        let tableExpanded = false;
        let chartsVisible = true;
        
        // Initialize charts
        function initCharts() {
            // Temperature Chart
            const tempCtx = document.getElementById('temperatureChart').getContext('2d');
            temperatureChart = new Chart(tempCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Temperature (°C)',
                        data: [],
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
                            backgroundColor: 'rgba(15, 23, 42, 0.95)',
                            titleColor: '#f8fafc',
                            bodyColor: '#f8fafc',
                            borderColor: '#ef4444',
                            borderWidth: 1,
                            cornerRadius: 12,
                            padding: 15,
                            callbacks: {
                                title: function(context) {
                                    return context[0].label;
                                },
                                label: function(context) {
                                    return \`\${context.dataset.label}: \${context.parsed.y}°C\`;
                                }
                            }
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
                                font: { size: 12 },
                                callback: function(value, index, values) {
                                    // Show only every 10th label for better readability
                                    if (index % Math.ceil(values.length / 10) === 0) {
                                        return value;
                                    }
                                    return '';
                                }
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
                    labels: [],
                    datasets: [{
                        label: 'Humidity (%)',
                        data: [],
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
                            backgroundColor: 'rgba(15, 23, 42, 0.95)',
                            titleColor: '#f8fafc',
                            bodyColor: '#f8fafc',
                            borderColor: '#3b82f6',
                            borderWidth: 1,
                            cornerRadius: 12,
                            padding: 15,
                            callbacks: {
                                label: function(context) {
                                    return \`\${context.dataset.label}: \${context.parsed.y}%\`;
                                }
                            }
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
                                font: { size: 12 },
                                callback: function(value, index, values) {
                                    if (index % Math.ceil(values.length / 10) === 0) {
                                        return value;
                                    }
                                    return '';
                                }
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
                    }
                }
            });
            
            // Pressure Chart
            const pressCtx = document.getElementById('pressureChart').getContext('2d');
            pressureChart = new Chart(pressCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Pressure (hPa)',
                        data: [],
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
                            backgroundColor: 'rgba(15, 23, 42, 0.95)',
                            titleColor: '#f8fafc',
                            bodyColor: '#f8fafc',
                            borderColor: '#10b981',
                            borderWidth: 1,
                            cornerRadius: 12,
                            padding: 15,
                            callbacks: {
                                label: function(context) {
                                    return \`\${context.dataset.label}: \${context.parsed.y} hPa\`;
                                }
                            }
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
                                font: { size: 12 },
                                callback: function(value, index, values) {
                                    if (index % Math.ceil(values.length / 10) === 0) {
                                        return value;
                                    }
                                    return '';
                                }
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
                    }
                }
            });
            
            startAutoUpdate();
        }
        
        // Update time display
        function updateTime() {
            const now = new Date();
            const timeStr = now.toLocaleTimeString([], { 
                hour12: false,
                hour: '2-digit', 
                minute: '2-digit', 
                second: '2-digit' 
            });
            const dateStr = now.toLocaleDateString([], { 
                weekday: 'long', 
                year: 'numeric', 
                month: 'long', 
                day: 'numeric' 
            });
            
            document.getElementById('currentTime').textContent = timeStr;
            document.getElementById('currentDate').textContent = dateStr;
            document.getElementById('serverTime').textContent = \`Server time: \${now.toLocaleString()}\`;
            
            // Update uptime
            const uptime = Date.now() - startTime;
            const hours = Math.floor(uptime / (1000 * 60 * 60));
            const minutes = Math.floor((uptime % (1000 * 60 * 60)) / (1000 * 60));
            const seconds = Math.floor((uptime % (1000 * 60)) / 1000);
            document.getElementById('uptime').textContent = \`\${hours}h \${minutes}m \${seconds}s\`;
        }
        
        // Get color based on value
        function getColorByValue(value, type) {
            if (type === 'temperature') {
                if (value < 10) return '#4cc9f0';
                else if (value < 20) return '#4361ee';
                else if (value < 25) return '#7209b7';
                else if (value < 30) return '#f72585';
                else return '#ff0000';
            } else if (type === 'humidity') {
                if (value < 30) return '#ff6b6b';
                else if (value < 60) return '#51cf66';
                else if (value < 80) return '#ffd43b';
                else return '#ff922b';
            } else if (type === 'pressure') {
                if (value < 1000) return '#748ffc';
                else if (value < 1020) return '#5c7cfa';
                else return '#364fc7';
            }
            return '#495057';
        }
        
        // Get status indicator
        function getStatus(value, type) {
            if (type === 'temperature') {
                if (value < 15) return { text: 'Cold', class: 'status-warn' };
                else if (value > 30) return { text: 'Hot', class: 'status-bad' };
                else return { text: 'Normal', class: 'status-good' };
            } else if (type === 'humidity') {
                if (value < 30) return { text: 'Dry', class: 'status-warn' };
                else if (value > 70) return { text: 'Humid', class: 'status-warn' };
                else return { text: 'Optimal', class: 'status-good' };
            } else if (type === 'pressure') {
                if (value < 990) return { text: 'Low', class: 'status-warn' };
                else if (value > 1030) return { text: 'High', class: 'status-warn' };
                else return { text: 'Normal', class: 'status-good' };
            }
            return { text: 'Normal', class: 'status-good' };
        }
        
        // Update data from server
        async function updateData() {
            try {
                const [historyRes, statsRes] = await Promise.all([
                    fetch('/api/history'),
                    fetch('/api/stats')
                ]);
                
                if (!historyRes.ok || !statsRes.ok) {
                    throw new Error('Failed to fetch data');
                }
                
                const historyData = await historyRes.json();
                const statsData = await statsRes.json();
                
                // Update statistics
                document.getElementById('tempCurrent').textContent = \`\${statsData.temperature.current.toFixed(1)}°C\`;
                document.getElementById('tempAvg').textContent = \`\${statsData.temperature.average.toFixed(1)}°C\`;
                document.getElementById('tempMin').textContent = \`\${statsData.temperature.min.toFixed(1)}°C\`;
                document.getElementById('tempMax').textContent = \`\${statsData.temperature.max.toFixed(1)}°C\`;
                
                document.getElementById('humCurrent').textContent = \`\${statsData.humidity.current.toFixed(1)}%\`;
                document.getElementById('humAvg').textContent = \`\${statsData.humidity.average.toFixed(1)}%\`;
                document.getElementById('humMin').textContent = \`\${statsData.humidity.min.toFixed(1)}%\`;
                document.getElementById('humMax').textContent = \`\${statsData.humidity.max.toFixed(1)}%\`;
                
                document.getElementById('pressCurrent').textContent = \`\${statsData.pressure.current.toFixed(1)} hPa\`;
                document.getElementById('pressAvg').textContent = \`\${statsData.pressure.average.toFixed(1)} hPa\`;
                document.getElementById('pressMin').textContent = \`\${statsData.pressure.min.toFixed(1)} hPa\`;
                document.getElementById('pressMax').textContent = \`\${statsData.pressure.max.toFixed(1)} hPa\`;
                
                // Update trends
                const updateTrend = (elementId, trend) => {
                    const element = document.getElementById(elementId);
                    element.className = \`trend \${trend}\`;
                    element.innerHTML = \`<i class="fas fa-arrow-\${trend === 'up' ? 'up' : trend === 'down' ? 'down' : 'right'}"></i> \${trend.charAt(0).toUpperCase() + trend.slice(1)}\`;
                };
                
                updateTrend('tempTrend', statsData.temperature.trend);
                updateTrend('humTrend', statsData.humidity.trend);
                updateTrend('pressTrend', statsData.pressure.trend);
                
                // Update charts
                const times = historyData.map(entry => {
                    const date = new Date(entry.time);
                    return date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
                });
                
                temperatureChart.data.labels = times;
                temperatureChart.data.datasets[0].data = historyData.map(d => d.temperature);
                
                humidityChart.data.labels = times;
                humidityChart.data.datasets[0].data = historyData.map(d => d.humidity);
                
                pressureChart.data.labels = times;
                pressureChart.data.datasets[0].data = historyData.map(d => d.pressure);
                
                temperatureChart.update('none');
                humidityChart.update('none');
                pressureChart.update('none');
                
                // Update table
                updateTable(historyData);
                
                // Update header stats
                document.getElementById('lastUpdate').textContent = new Date(statsData.lastUpdate).toLocaleTimeString();
                document.getElementById('dataPoints').textContent = \`\${statsData.count} records\`;
                document.getElementById('status').innerHTML = '<span style="color: #10b981">Live Streaming</span>';
                document.getElementById('connectionStatus').textContent = 'Connected';
                
            } catch (error) {
                console.error('Error updating data:', error);
                document.getElementById('status').innerHTML = '<span style="color: #ef4444">Connection Lost</span>';
                document.getElementById('connectionStatus').textContent = 'Disconnected';
            }
        }
        
        // Update data table
        function updateTable(data) {
            const tableBody = document.getElementById('tableBody');
            const displayData = tableExpanded ? data.slice(-50).reverse() : data.slice(-10).reverse();
            
            tableBody.innerHTML = '';
            
            if (displayData.length === 0) {
                tableBody.innerHTML = \`
                    <tr>
                        <td colspan="7" style="text-align: center; padding: 50px; color: #94a3b8;">
                            <i class="fas fa-database" style="font-size: 2rem; margin-bottom: 10px; display: block;"></i>
                            No data available yet
                        </td>
                    </tr>
                \`;
                return;
            }
            
            displayData.forEach((entry, index) => {
                const date = new Date(entry.time);
                const timeStr = date.toLocaleTimeString([], { 
                    hour12: false,
                    hour: '2-digit', 
                    minute: '2-digit', 
                    second: '2-digit' 
                });
                const dateStr = date.toLocaleDateString([], { 
                    day: '2-digit', 
                    month: '2-digit', 
                    year: 'numeric' 
                });
                
                const tempStatus = getStatus(entry.temperature, 'temperature');
                const humStatus = getStatus(entry.humidity, 'humidity');
                const pressStatus = getStatus(entry.pressure, 'pressure');
                
                // Determine overall status
                let overallStatus = tempStatus;
                if (tempStatus.class === 'status-bad' || humStatus.class === 'status-bad' || pressStatus.class === 'status-bad') {
                    overallStatus = { text: 'Alert', class: 'status-bad' };
                } else if (tempStatus.class === 'status-warn' || humStatus.class === 'status-warn' || pressStatus.class === 'status-warn') {
                    overallStatus = { text: 'Warning', class: 'status-warn' };
                }
                
                // Calculate trend
                let trend = 'stable';
                if (index > 0 && displayData[index - 1]) {
                    const prevEntry = displayData[index - 1];
                    const tempDiff = entry.temperature - prevEntry.temperature;
                    if (Math.abs(tempDiff) > 0.5) {
                        trend = tempDiff > 0 ? 'up' : 'down';
                    }
                }
                
                const row = document.createElement('tr');
                row.innerHTML = \`
                    <td>\${dateStr}</td>
                    <td>\${timeStr}</td>
                    <td style="color: \${getColorByValue(entry.temperature, 'temperature')}; font-weight: 600;">\${entry.temperature.toFixed(1)}</td>
                    <td style="color: \${getColorByValue(entry.humidity, 'humidity')}; font-weight: 600;">\${entry.humidity.toFixed(1)}</td>
                    <td style="color: \${getColorByValue(entry.pressure, 'pressure')}; font-weight: 600;">\${entry.pressure.toFixed(1)}</td>
                    <td>
                        <span class="status-indicator \${overallStatus.class}">
                            <i class="fas fa-\${overallStatus.class === 'status-good' ? 'check' : overallStatus.class === 'status-warn' ? 'exclamation-triangle' : 'exclamation-circle'}"></i>
                            \${overallStatus.text}
                        </span>
                    </td>
                    <td>
                        <span class="trend \${trend}">
                            <i class="fas fa-arrow-\${trend === 'up' ? 'up' : trend === 'down' ? 'down' : 'right'}"></i>
                            \${trend.charAt(0).toUpperCase() + trend.slice(1)}
                        </span>
                    </td>
                \`;
                tableBody.appendChild(row);
            });
        }
        
        // Toggle table view
        function toggleTable() {
            tableExpanded = !tableExpanded;
            const btn = document.getElementById('tableToggle');
            btn.innerHTML = tableExpanded ? 
                '<i class="fas fa-eye-slash"></i> Show Less' : 
                '<i class="fas fa-eye"></i> Show All';
            
            fetch('/api/history')
                .then(res => res.json())
                .then(data => updateTable(data))
                .catch(console.error);
        }
        
        // Toggle charts visibility
        function toggleCharts() {
            chartsVisible = !chartsVisible;
            const chartCards = document.querySelectorAll('.chart-card');
            chartCards.forEach(card => {
                card.style.display = chartsVisible ? 'flex' : 'none';
            });
            showNotification(chartsVisible ? 'Charts shown' : 'Charts hidden', 'info');
        }
        
        // Auto update control
        function startAutoUpdate() {
            if (autoUpdateInterval) clearInterval(autoUpdateInterval);
            autoUpdateInterval = setInterval(() => {
                updateData();
                updateTime();
            }, 5000);
            autoUpdateEnabled = true;
            document.getElementById('autoUpdateBtn').innerHTML = '<i class="fas fa-pause"></i> Pause Auto Update';
            showNotification('Auto update enabled (5s interval)', 'success');
        }
        
        function stopAutoUpdate() {
            if (autoUpdateInterval) clearInterval(autoUpdateInterval);
            autoUpdateEnabled = false;
            document.getElementById('autoUpdateBtn').innerHTML = '<i class="fas fa-play"></i> Auto Update (5s)';
            showNotification('Auto update paused', 'warning');
        }
        
        function toggleAutoUpdate() {
            if (autoUpdateEnabled) {
                stopAutoUpdate();
            } else {
                startAutoUpdate();
            }
        }
        
        // Utility functions
        function refreshAllData() {
            updateData();
            showNotification('Data refreshed successfully', 'success');
        }
        
        function refreshTable() {
            fetch('/api/history')
                .then(res => res.json())
                .then(data => {
                    updateTable(data);
                    showNotification('Table refreshed', 'success');
                })
                .catch(console.error);
        }
        
        function exportData() {
            window.location.href = '/api/export';
            showNotification('Exporting data to CSV...', 'info');
        }
        
        async function clearData() {
            if (confirm('⚠️ Are you sure you want to clear ALL data? This action cannot be undone.')) {
                try {
                    const response = await fetch('/api/clear', { method: 'POST' });
                    const result = await response.json();
                    if (result.status === 'success') {
                        showNotification('All data cleared successfully!', 'success');
                        setTimeout(refreshAllData, 500);
                    }
                } catch (error) {
                    showNotification('Error clearing data: ' + error.message, 'error');
                }
            }
        }
        
        function resetDashboard() {
            if (confirm('🔄 Are you sure you want to reset the dashboard? This will refresh all components.')) {
                location.reload();
            }
        }
        
        // Notification system
        function showNotification(message, type = 'info') {
            const notification = document.createElement('div');
            notification.style.cssText = \`
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
                display: flex;
                align-items: center;
                gap: 12px;
            \`;
            
            const icons = {
                success: 'check-circle',
                error: 'exclamation-circle',
                warning: 'exclamation-triangle',
                info: 'info-circle'
            };
            
            const colors = {
                success: 'linear-gradient(135deg, rgba(16, 185, 129, 0.9), rgba(5, 150, 105, 0.9))',
                error: 'linear-gradient(135deg, rgba(239, 68, 68, 0.9), rgba(220, 38, 38, 0.9))',
                warning: 'linear-gradient(135deg, rgba(245, 158, 11, 0.9), rgba(217, 119, 6, 0.9))',
                info: 'linear-gradient(135deg, rgba(59, 130, 246, 0.9), rgba(37, 99, 235, 0.9))'
            };
            
            notification.style.background = colors[type] || colors.info;
            notification.innerHTML = \`
                <i class="fas fa-\${icons[type] || 'info-circle'}"></i>
                \${message}
            \`;
            
            document.body.appendChild(notification);
            
            setTimeout(() => {
                notification.style.animation = 'slideOut 0.3s ease';
                setTimeout(() => notification.remove(), 300);
            }, 3000);
        }
        
        // Add CSS for notifications
        const style = document.createElement('style');
        style.textContent = \`
            @keyframes slideIn {
                from { transform: translateX(100%); opacity: 0; }
                to { transform: translateX(0); opacity: 1; }
            }
            @keyframes slideOut {
                from { transform: translateX(0); opacity: 1; }
                to { transform: translateX(100%); opacity: 0; }
            }
        \`;
        document.head.appendChild(style);
        
        // Initialize everything
        document.addEventListener('DOMContentLoaded', function() {
            initCharts();
            updateTime();
            updateData();
            setInterval(updateTime, 1000);
            showNotification('Dashboard initialized successfully!', 'success');
        });
        
        // Handle visibility change
        document.addEventListener('visibilitychange', function() {
            if (document.hidden) {
                console.log('Page hidden, pausing auto update');
                if (autoUpdateEnabled) stopAutoUpdate();
            } else {
                console.log('Page visible, resuming auto update');
                if (autoUpdateEnabled) startAutoUpdate();
            }
        });
    </script>
</body>
</html>
`);
});

// ================= START =================

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`🚀 Server running on port ${PORT}`);
  console.log(`📊 Dashboard available at http://localhost:${PORT}`);
  console.log(`📤 POST data to http://localhost:${PORT}/api/data`);
});