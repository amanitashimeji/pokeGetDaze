# include <Siv3D.hpp> // OpenSiv3D v0.6.3



namespace/*関数用*/ {
	uint16 Pokemon_ExistLV_HP() {}
	float Pokemon_Capture_Method() {}
	template<typename T>
	T Percentage() {}
};


namespace /*構造体*/ {

	struct Status {
		uint16 SyuzokuHP;
		uint16 CatchRATE;
	};

	struct ActArgmentList {
		uint16 MaxHP;
		bool HPFlag;
		uint16 CatchRATE;
		float BoalRATE;
		float StatusRATE;
	};


};


uint16 Pokemon_ExistLV_HP(const uint16& SHP, const uint16& LV, const uint16& KoHP) {

	//constexpr uint16 DoryokuHP = 0;//今回は野生ポケモンなので0


	return ((((SHP * 2) + KoHP) * LV / 100) + (LV + 10));

}


float Pokemon_Capture_Method(const ActArgmentList &a) {
	uint16 MinHP = a.MaxHP * 2;
	if (a.HPFlag) {
		MinHP = 2;
	}
	return (((a.MaxHP*3)- MinHP) * a.CatchRATE * a.BoalRATE / (a.MaxHP*3)) * a.StatusRATE;

}

template<typename T>
T Percentage(T a, T b)//100分率表記になおす
{
	return a / b * 100;
}




void Main()

{

	String VersionInfo = U"ポケモン捕獲率計算ver.0.02";
	Window::SetTitle(VersionInfo);
	Window::Resize(420, 600);


	//PokemonDate.csvを読み込む処理　
	
	const CSV csv{ U"./PokemonDate.csv" };//csv読み込み処理
	if (not csv) {
		throw Error{ U"Failed to load `PokemonDate.csv`" };//読み込めなかった場合のエラー処理
	}

	HashTable<String, uint16>PokemonNameList; //<ポケモン名,図鑑No>
	Array<Status>PokemonDate;//PokemonDate[i].HP PokemonDate[i].CR

	for (auto i : step(csv.rows())) {//csv→Hash,Array

		//行サイズをcsv.rows()で得ているので
		//形式に沿ってPokemonDate.csvに項目を追加すれば,exeを変えること無く最新データに対応可

		//NameTable.contains(U---);完全一致するテキストの検索;
		//NameTable[U--];一致テキストのキー(今回は図鑑No兼行番号);
		PokemonNameList.emplace(csv[i][1], i);
		Status kari;
		kari.SyuzokuHP = Parse<uint16>(csv[i][2]);
		kari.CatchRATE = Parse<uint16>(csv[i][3]);
		PokemonDate << kari;
	}

	//読み込み終了



	TextEditState tex01;//ユーザー入力ポケモン名
	TextEditState tex02;//ユーザー入力レベル(String型)

	bool avtivateNextTextBox = false;

	uint16 Level = 50;//tex02.text(Parse<uint16>),実HP処理関数にわたす


	bool KeisanCan = false;//入力値を見て計算可能か判断するフラグ


	const Array<String>HPdate = { U"まんたん",U"のこり1" };//選択肢配列
	const Array<String>BOAL = {
		U"モンスターボール",
		U"スーパーボール",
		U"ハイパーボール",
		U"クイックボール",
		U"ダークボール",
		U"リピートボール",
		U"タイマーボール",
	};
	const Array<String>Sick = { U"なにもしない",U"まひ",U"ねむり" };
	size_t index0 = 0;//残りHP選択肢
	size_t index1 = 0;//使うボール選択肢
	size_t index2 = 0;//状態異常

	const Font font{ 25 ,Typeface::CJK_Regular_JP };//計算結果描画
	float UnderHP_Result = 0;//HP個体値0の時の捕獲確率(100分率表記済み)
	float UpperHP_Result = 0;//HP個体値31のときの捕獲確率(percentage)

	ActArgmentList PokemonArgment{1,false,3,1,1};

	while (System::Update())// メインループ
	{
		ClearPrint();
		if (avtivateNextTextBox)
		{
			// テキストボックスをアクティブ化
			tex02.active = true;

			avtivateNextTextBox = false;
		}
		//Print << Cursor::Pos();//GUI配置のためのマウスカーソル座標
		font(VersionInfo).draw(40, 20);
		font(U"推定捕獲率").draw(220, 80);
		font(U"{:.4f}%\n     ～{:.4f}%"_fmt(UnderHP_Result, UpperHP_Result)).draw(210, 120);//小数点以下4桁まで表示する


		const bool previous = tex01.active;
		SimpleGUI::Headline(U"ポケモン名", Vec2{ 20, 80 });
		SimpleGUI::TextBox(tex01, Vec2{ 18, 115 }, 130);

		// 非アクティブ化された
		if (previous && (tex01.active == false))
		{
			// Tab キーが入力されていた場合、次のテキストボックスをアクティブ化するフラグを true に
			avtivateNextTextBox = TextInput::GetRawInput().includes(U'\t');
		}

		SimpleGUI::Headline(U"レベル", Vec2{ 20, 160 }, 80);
		SimpleGUI::TextBox(tex02, Vec2{ 18, 195 }, 100);
		Level = ParseOr<uint16>(tex02.text, 0);


		SimpleGUI::Headline(U"残りHP", Vec2{ 20, 260 });
		if (SimpleGUI::RadioButtons(index0, HPdate, Vec2{ 20, 300 })) {
			if (index0 == 1) {
				PokemonArgment.HPFlag = true;
			}
			else if (index0 == 0) {
				PokemonArgment.HPFlag = false;
			}
		}

		SimpleGUI::Headline(U"使うボール", Vec2{ 200, 230 });
		if (SimpleGUI::RadioButtons(index1, BOAL, Vec2{ 200, 270 })) {
			switch (index1) {
			case 0://モンスターボール
				PokemonArgment.BoalRATE = 1.0F;
				break;
			case 1://スーパーボール
				PokemonArgment.BoalRATE = 1.5F;
				break;
			case 2://ハイパーボール
				PokemonArgment.BoalRATE = 2.0F;
				break;
			case 3://クイックボール
				PokemonArgment.BoalRATE = 5.0F;
				break;
			case 4://ダークボール
			case 5://リピートボール //faulthrough
				PokemonArgment.BoalRATE = 3.0F;
				break;
			case 6://タイマーボール10T以降
				PokemonArgment.BoalRATE = 4.0F;
				break;
			}
		}

		SimpleGUI::Headline(U"状態異常", Vec2{ 20, 390 });
		if (SimpleGUI::RadioButtons(index2, Sick, Vec2{ 20, 430 })) {
			switch (index2) {
			case 0://状態異常なし
				PokemonArgment.StatusRATE = 1.0F;
				break;
			case 1://まひどくやけど
				PokemonArgment.StatusRATE = 1.5F;
				break;
			case 2://ねむりこおり
				PokemonArgment.StatusRATE = 2.5F;
				break;
			}
		}

		if (PokemonNameList.contains(tex01.text) && Level > 0 && Level <= 100) {
			//入力名が有効かつLevelが有効な数字であるなら
			KeisanCan = true;//捕獲率計算ボタンを有効にする		
		}
		else {
			KeisanCan = false;//そうでないならば計算させない
		}

		if (SimpleGUI::Button(U"捕獲率を計算する", Vec2{ 200,555 }, unspecified, KeisanCan)) {
			PokemonArgment.CatchRATE = PokemonDate[PokemonNameList[tex01.text]].CatchRATE;

			//HP種族値0の時
			PokemonArgment.MaxHP= Pokemon_ExistLV_HP(PokemonDate[PokemonNameList[tex01.text]].SyuzokuHP, Level, 0);
			UnderHP_Result = Percentage(Pokemon_Capture_Method(PokemonArgment), 255.0F);


			//HP種族値31の時
			PokemonArgment.MaxHP = Pokemon_ExistLV_HP(PokemonDate[PokemonNameList[tex01.text]].SyuzokuHP, Level, 31);
			UpperHP_Result = Percentage(Pokemon_Capture_Method(PokemonArgment), 255.0F);

		}
		// "Licenses" ボタンが押されたら
		if (SimpleGUI::Button(U"Licenses", Vec2{ 20, 555 }))
		{
			// ライセンス情報を表示
			LicenseManager::ShowInBrowser();
		}

	}
}
