import os
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# 1. 讀取台積電 10 年行情數據
price_file = "data.csv"
if not os.path.exists(price_file) and os.path.exists(os.path.join("..", price_file)):
    price_file = os.path.join("..", price_file)

df_price = pd.read_csv(price_file)
df_price['Date'] = pd.to_datetime(df_price['Date'])
df_price.sort_values('Date', inplace=True)

# 2. 定義三個策略的交易報表與對應顏色
strategy_configs = [
    {
        "name": "BreakoutStrategy",
        "file": os.path.join("trades", "BreakoutStrategy_trade.csv"),
        "buy_color": "#00CC96",    
        "sell_color": "#EF553B",   
        "default_visible": True    
    },
    {
        "name": "KellyMAStrategy",
        "file": os.path.join("trades", "KellyMAStrategy_trade.csv"),
        "buy_color": "#636EFA",    
        "sell_color": "#FFA15A",   
        "default_visible": "legendonly"  
    },
    {
        "name": "DipBuyerStrategy",
        "file": os.path.join("trades", "DipBuyerStrategy_trade.csv"),
        "buy_color": "#AB63FA",   
        "sell_color": "#E377C2",  
        "default_visible": "legendonly"  
    }
]

# 3. 建立雙層畫布（上方 K 線 75%，下方成交量 25%）
fig = make_subplots(
    rows=2, cols=1,
    shared_xaxes=True,
    vertical_spacing=0.03,
    subplot_titles=('台積電 (2330.TW) 10年多策略買賣訊號檢視器 (點擊右側圖例切換策略)', '成交量 (Volume)'),
    row_heights=[0.75, 0.25]
)

# 4. 底層：K 線燭台圖（台股習慣：紅漲綠跌）
fig.add_trace(
    go.Candlestick(
        x=df_price['Date'],
        open=df_price['Open'],
        high=df_price['High'],
        low=df_price['Low'],
        close=df_price['Close'],
        name='台積電 K 線',
        increasing_line_color='red',
        decreasing_line_color='green'
    ),
    row=1, col=1
)

# 5. 底層：成交量直方圖
fig.add_trace(
    go.Bar(
        x=df_price['Date'],
        y=df_price['Volume'],
        name='成交量',
        marker_color='rgba(150, 150, 150, 0.4)',
        showlegend=False
    ),
    row=2, col=1
)

# 6. 依序載入 3 個策略的訊號點
for strat in strategy_configs:
    fpath = strat["file"]
    # 支援路徑相容
    if not os.path.exists(fpath):
        fpath = os.path.join("build", strat["file"])
    
    if not os.path.exists(fpath):
        print(f"找不到報表：{strat['file']}，已跳過。")
        continue

    df_trades = pd.read_csv(fpath)
    if df_trades.empty:
        continue

    df_trades['Date'] = pd.to_datetime(df_trades['Date'])
    buys = df_trades[df_trades['Type'] == 'BUY']
    sells = df_trades[df_trades['Type'] == 'SELL']

    vis = strat["default_visible"]

    # 買進標記 (▲)
    fig.add_trace(
        go.Scatter(
            x=buys['Date'],
            y=buys['Price'] * 0.97,
            mode='markers',
            marker=dict(symbol='triangle-up', size=11, color=strat["buy_color"], line=dict(width=1, color='black')),
            name=f'{strat["name"]} (買進 ▲)',
            visible=vis,
            hovertext=[f"【{strat['name']} 買入】<br>日期: {d.strftime('%Y-%m-%d')}<br>成交價: {p}<br>股數: {s}" 
                       for d, p, s in zip(buys['Date'], buys['Price'], buys['Shares'])],
            hoverinfo='text'
        ),
        row=1, col=1
    )

    # 賣出標記 (▼)
    fig.add_trace(
        go.Scatter(
            x=sells['Date'],
            y=sells['Price'] * 1.03,
            mode='markers',
            marker=dict(symbol='triangle-down', size=11, color=strat["sell_color"], line=dict(width=1, color='black')),
            name=f'{strat["name"]} (賣出 ▼)',
            visible=vis,
            hovertext=[f"【{strat['name']} 賣出】<br>日期: {d.strftime('%Y-%m-%d')}<br>成交價: {p}<br>已實現損益: {r:,.0f}" 
                       for d, p, r in zip(sells['Date'], sells['Price'], sells['RealizedProfit'])],
            hoverinfo='text'
        ),
        row=1, col=1
    )

# 7. 版面與時間軸拉桿設定
# 更新 X 軸與 Y 軸互動行為
# 1. 徹底關閉上方與下方的拉桿 (Range Slider)，解除成交量被遮蔽的 Bug
fig.update_xaxes(rangeslider_visible=False, row=1, col=1)
fig.update_xaxes(rangeslider_visible=False, row=2, col=1)

# 2. 解除 Y 軸限制，並讓 Y 軸保留 10% 的頂部與底部呼吸空間 (避免 K 棒頂破天花板)
fig.update_yaxes(fixedrange=False, autorange=True, row=1, col=1)
fig.update_yaxes(fixedrange=False, autorange=True, row=2, col=1)

# 3. 仿 TradingView 手感配置
fig.update_layout(
    height=850,
    template='plotly_white',
    dragmode='pan',              # 預設模式：按住滑鼠左鍵可「左右上下無死角平移」
    hovermode='closest',         # 只有滑鼠游標碰到實體或三角形時才跳出小提示，不霸佔大版面
    margin=dict(l=60, r=160, t=60, b=40),  # 右側多留空間給圖例，避免撞車
    legend=dict(
        orientation="v",
        yanchor="top",
        y=1.0,
        xanchor="left",
        x=1.02,
        bgcolor="rgba(255,255,255,0.85)",
        bordercolor="#e0e0e0",
        borderwidth=1
    )
)

# 4. 關鍵：在輸出 HTML 時啟用「滑鼠滾輪自由縮放 (scrollZoom)」
config = {
    'scrollZoom': True,          # 啟用滾輪縮放 (不用再去拉什麼框框或拉桿)
    'displayModeBar': True,       # 顯示上方小工具列
    'displaylogo': False,        # 移除 Plotly Logo
    'modeBarButtonsToRemove': ['select2d', 'lasso2d']
}

output_html = "multi_strategy_signals.html"
fig.write_html(output_html, config=config)
print(f"🎉 交易檢視器已產出 ：{output_html} (正在打開瀏覽器...)")
fig.show(config=config)