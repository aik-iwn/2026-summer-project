import os
import pandas as pd
import matplotlib.pyplot as plt

# 1. 解決 Windows 中文變成豆腐方塊 (設定微軟正黑體)
plt.rcParams['font.sans-serif'] = ['Microsoft JhengHei', 'SimHei', 'Arial']
plt.rcParams['axes.unicode_minus'] = False  # 解決負號 '-' 變方塊的問題

# 2. 定義策略名單與顏色樣式
strategies = [
    {"name": "DipBuyerStrategy", "file": "DipBuyerStrategy_report.csv", "color": "#7f7f7f", "style": "--"},
    {"name": "KellyMAStrategy",  "file": "KellyMAStrategy_report.csv",  "color": "#2ca02c", "style": "-"},
    {"name": "BreakoutStrategy", "file": "BreakoutStrategy_report.csv", "color": "#1f77b4", "style": "-"}
]

initial_capital = 1500000.0

# 建立畫布
fig, (ax1, ax2) = plt.subplots(nrows=2, ncols=1, figsize=(13, 8), sharex=True, 
                               gridspec_kw={'height_ratios': [2.5, 1]})

plotted_any = False

for strat in strategies:
    fname = strat["file"]
    
    # 智慧搜尋檔案位置：當前目錄 -> build 目錄 -> reports 目錄
    target_path = None
    search_paths = [
        fname,
        os.path.join("build", fname),
        os.path.join("reports", fname),
        os.path.join("..", fname)
    ]
    for p in search_paths:
        if os.path.exists(p):
            target_path = p
            break

    if target_path is None:
        print(f"❌ 找不到報表：{fname}，請確認 C++ 是否已執行輸出！")
        continue

    print(f"✅ 成功載入報表：{target_path}")
    df = pd.read_csv(target_path)
    if df.empty:
        print(f"⚠️ {target_path} 內容為空，跳過。")
        continue

    plotted_any = True
    df['Date'] = pd.to_datetime(df['Date'])
    df.sort_values('Date', inplace=True)

    # 計算累計已實現損益、資產與回撤
    df['CumulativeProfit'] = df['RealizedProfit'].cumsum()
    df['Equity'] = initial_capital + df['CumulativeProfit']
    df['Peak'] = df['Equity'].cummax()
    df['Drawdown(%)'] = (df['Equity'] - df['Peak']) / df['Peak'] * 100.0

    final_roi = (df['Equity'].iloc[-1] - initial_capital) / initial_capital * 100.0
    max_mdd = df['Drawdown(%)'].min()

    # 繪製上方資產走勢
    ax1.plot(df['Date'], df['Equity'], 
             label=f"{strat['name']} (最終 ROI: {final_roi:+.1f}%)", 
             color=strat["color"], linestyle=strat["style"], linewidth=2.0)

    # 繪製下方水下回撤
    ax2.plot(df['Date'], df['Drawdown(%)'], 
             label=f"{strat['name']} (MDD: {abs(max_mdd):.1f}%)", 
             color=strat["color"], linewidth=1.2, alpha=0.8)

# 基準線與排版設定
ax1.axhline(initial_capital, color='black', linestyle=':', label=f'初始本金 ({int(initial_capital):,} 元)', alpha=0.7)
ax1.set_title('三大量化策略 10 年回測巔峰對決 (Equity & Drawdown Comparison)', fontsize=14, fontweight='bold')
ax1.set_ylabel('帳戶資產總額 (NTD)', fontsize=11)
ax1.grid(True, linestyle=':', alpha=0.6)
ax1.legend(loc='upper left', frameon=True)

ax2.set_title('水下回撤風險對比 (Underwater Risk Exposure)', fontsize=11)
ax2.set_ylabel('回撤率 (%)', fontsize=11)
ax2.set_xlabel('交易日期', fontsize=11)
ax2.grid(True, linestyle=':', alpha=0.6)
ax2.legend(loc='lower left', frameon=True)

plt.tight_layout()

if plotted_any:
    output_img = "strategy_comparison.png"
    plt.savefig(output_img, dpi=300)
    print(f"\n🎉 三大策略對決圖已成功生成並存檔至：{output_img}")
    plt.show()
else:
    print("\n⚠️ 未找到任何策略報表，請先至 C++ 執行選單選項 4 產出 CSV 報表！")