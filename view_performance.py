import os
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

initial_capital = 1500000.0  # 初始本金 150 萬

strategy_configs = [
    {
        "name": "DipBuyerStrategy",
        "file": os.path.join("equity", "DipBuyerStrategy_equity.csv"),
        "color": "#AB63FA",
        "dash": "dash"
    },
    {
        "name": "KellyMAStrategy",
        "file": os.path.join("equity", "KellyMAStrategy_equity.csv"),
        "color": "#00CC96",
        "dash": "solid"
    },
    {
        "name": "BreakoutStrategy",
        "file": os.path.join("equity", "BreakoutStrategy_equity.csv"),
        "color": "#1f77b4",
        "dash": "solid"
    }
]

# 建立上下雙層子圖 (上圖：真實資產 70%，下圖：水下回撤 30%)
fig = make_subplots(
    rows=2, cols=1,
    shared_xaxes=True,
    vertical_spacing=0.04,
    subplot_titles=('10 年真實資產成長曲線 (Mark-to-Market vs Benchmark)', '水下回撤風險歷程 (Underwater Risk Exposure %)'),
    row_heights=[0.7, 0.3]
)

benchmark_plotted = False

for strat in strategy_configs:
    fpath = strat["file"]
    if not os.path.exists(fpath):
        fpath = os.path.join("build", strat["file"])
    if not os.path.exists(fpath):
        continue

    df = pd.read_csv(fpath)
    if df.empty or 'TotalEquity' not in df.columns:
        continue

    df['Date'] = pd.to_datetime(df['Date'])
    df.sort_values('Date', inplace=True)

    # 1. 繪製台積電 Buy & Hold 基準線 (只繪製一次，上下同步聯動)
    if not benchmark_plotted and 'Close' in df.columns:
        benchmark_equity = (df['Close'] / df['Close'].iloc[0]) * initial_capital
        bench_roi = (benchmark_equity.iloc[-1] - initial_capital) / initial_capital * 100.0
        
        # 計算大盤的每日回撤
        bench_cummax = benchmark_equity.cummax()
        bench_dd = (benchmark_equity - bench_cummax) / bench_cummax * 100.0
        bench_mdd = abs(bench_dd.min())
        
        # 上圖：大盤資產走勢 (保留單一圖例)
        fig.add_trace(
            go.Scatter(
                x=df['Date'],
                y=benchmark_equity,
                name=f"TSMC Buy & Hold (ROI: {bench_roi:+.1f}%, MDD: {bench_mdd:.1f}%)",
                legendgroup="TSMC",  # 【關鍵】設定群組名稱
                line=dict(color="#FFA15A", dash="dot", width=1.8),
                hovertemplate="日期: %{x|%Y-%m-%d}<br>基準總值: NT$ %{y:,.0f}<extra></extra>"
            ),
            row=1, col=1
        )
        
        # 下圖：大盤水下回撤線 (隱藏圖例，但綁定同一個群組以同步開關)
        fig.add_trace(
            go.Scatter(
                x=df['Date'],
                y=bench_dd,
                name="TSMC 死抱回撤",
                legendgroup="TSMC",  # 【關鍵】綁定同一個群組
                showlegend=False,    # 【關鍵】不顯示第二個名字
                line=dict(color="#FFA15A", dash="dot", width=1.5),
                hovertemplate="<b>TSMC 死抱回撤</b><br>日期: %{x|%Y-%m-%d}<br>回撤幅度: %{y:.2f}%<extra></extra>"
            ),
            row=2, col=1
        )
        
        benchmark_plotted = True

    # 2. 計算各策略績效
    final_equity = df['TotalEquity'].iloc[-1]
    roi = (final_equity - initial_capital) / initial_capital * 100.0

    if 'DrawdownPct' in df.columns:
        dd_series = -abs(df['DrawdownPct'])
    else:
        cummax = df['TotalEquity'].cummax()
        dd_series = (df['TotalEquity'] - cummax) / cummax * 100.0
    mdd = abs(dd_series.min())

    group_id = strat['name']

    # 3. 上圖：資產淨值線 (保留單一圖例)
    fig.add_trace(
        go.Scatter(
            x=df['Date'],
            y=df['TotalEquity'],
            name=f"{strat['name']} (ROI: {roi:+.1f}%, MDD: {mdd:.1f}%)",
            legendgroup=group_id,  # 【關鍵】群組同步
            line=dict(color=strat['color'], dash=strat['dash'], width=2),
            hovertemplate=f"<b>{strat['name']}</b><br>日期: %{{x|%Y-%m-%d}}<br>總資產: NT$ %{{y:,.0f}}<extra></extra>"
        ),
        row=1, col=1
    )

    # 4. 下圖：水下回撤圖 (隱藏圖例，綁定同一個群組)
    fig.add_trace(
        go.Scatter(
            x=df['Date'],
            y=dd_series,
            name=f"{strat['name']} 回撤",
            legendgroup=group_id,  # 【關鍵】群組同步
            showlegend=False,      # 【關鍵】不重複佔用圖例空間
            line=dict(color=strat['color'], width=1.2),
            fill='tozeroy',
            fillcolor=strat['color'].replace(")", ", 0.15)").replace("rgb", "rgba").replace("#1f77b4", "rgba(31, 119, 180, 0.15)").replace("#00CC96", "rgba(0, 204, 150, 0.15)").replace("#AB63FA", "rgba(171, 99, 250, 0.15)"),
            hovertemplate=f"<b>{strat['name']} 回撤</b><br>日期: %{{x|%Y-%m-%d}}<br>幅度: %{{y:.2f}}%<extra></extra>"
        ),
        row=2, col=1
    )

# 基準本金參考線 (150萬)
fig.add_hline(
    y=initial_capital, line_dash="dash", line_color="gray",
    annotation_text=f"初始本金 ({int(initial_capital):,} 元)",
    row=1, col=1
)
fig.add_hline(y=0, line_color="black", line_width=1, row=2, col=1)

# X 軸與 Y 軸設定
fig.update_xaxes(rangeslider_visible=False, fixedrange=False, row=1, col=1)
fig.update_xaxes(rangeslider_visible=False, fixedrange=False, row=2, col=1)
fig.update_yaxes(fixedrange=False, autorange=True, row=1, col=1)
fig.update_yaxes(fixedrange=False, autorange=True, row=2, col=1)

# 佈局與手感配置
fig.update_layout(
    height=850,
    template='plotly_white',
    dragmode='pan',
    hovermode='x unified',
    margin=dict(l=60, r=60, t=60, b=40),
    legend=dict(
        orientation="h",
        yanchor="bottom",
        y=1.02,
        xanchor="right",
        x=1.0,
        groupclick="togglegroup"  # 【關鍵】點擊圖例項時整個群組一起開關
    )
)

config = {
    'scrollZoom': True,
    'displayModeBar': True,
    'displaylogo': False,
    'modeBarButtonsToRemove': ['select2d', 'lasso2d']
}

output_html = "portfolio_performance.html"
fig.write_html(output_html, config=config)
print(f"🎉 互動式績效儀表板已更新：{output_html} (正在打開瀏覽器...)")
fig.show(config=config)