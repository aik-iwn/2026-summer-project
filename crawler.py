import yfinance as yf
import pandas as pd

ticker="2330.TW"
period_str="10y"
stock=yf.Ticker(ticker)
data=stock.history(period=period_str)
data.reset_index(inplace=True)#將原本為索引值的日期列為新的一欄(inplace=True代表直接在原地修改)
data['Date']=data['Date'].dt.strftime('%Y-%m-%d')#將原本Date欄的輸出格式改成這樣子
data=data[['Date','Open','High','Low','Close','Volume']]#把data的資料只留下這幾欄
data.dropna(inplace=True)#把Nan(沒有交易資訊)的row刪掉
data.to_csv("data.csv",index=False)#將資料轉成csv檔並輸出，然後不必輸出索引值