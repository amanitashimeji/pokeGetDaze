# include <Siv3D.hpp> // OpenSiv3D v0.6.3


uint16 Pokemon_ExistLV_HP(const uint16& SHP, const uint16& LV, const uint16& KoHP) {

	//constexpr uint16 DoryokuHP = 0;//今回は野生ポケモンなので0

	uint16 Answer = ((((SHP * 2) + KoHP) * LV / 100) + (LV + 10));

	return Answer;

}


template<typename T>
T Percentage(T a, T b) {//100分率になおす aは対象bは基準
	return a / b * 100;
}


float PCMTHOD(const uint16& a, const bool& b, const uint16& CatchRATE, const float& BoalRATE, const float& StatusRATE) {//ver0.01で使っている
	const uint16 MaxHP = a * 3;
	uint16 MinHP = a * 2;
	if (b) {
		MinHP = 1;
	}
	return ((MaxHP - MinHP) * CatchRATE * BoalRATE / MaxHP) * StatusRATE;


}



struct PokeDate {
	uint16 SyuzokuHP;
	uint16 CatchRATE;
};

void Main()

{
	Window::SetTitle(U"ポケモン捕獲率計算ver.0.01");
	Window::Resize(420, 600);

	const CSV csv{ U"./PokemonDate.csv" };//csv読み込み処理
	if (not csv) {
		throw Error{ U"Failed to load `PokemonDate.csv`" };//読み込めなかった場合のエラー処理
	}

	HashTable<String, uint16>NameTable; //No=Name
	Array<PokeDate>PokeValue;//PokeValue[i].HP PokeValue[i].CR

	for (auto i : step(csv.rows())) {//csv→Hash,Array
		//NameTable.contains(U---);完全一致するテキストの検索;
		//NameTable[U--];一致テキストのキー(今回は図鑑No兼行番号);
		NameTable.emplace(csv[i][1], i);
		PokeDate kari;
		kari.SyuzokuHP = Parse<uint16>(csv[i][2]);
		kari.CatchRATE = Parse<uint16>(csv[i][3]);
		PokeValue << kari;
	}



	TextEditState tex01;//ユーザー入力ポケモン名
	TextEditState tex02;//ユーザー入力レベル(String型)

	bool avtivateNextTextBox = false;

	uint16 Level = 50;//tex02.text(Parse<uint16>),実HP処理関数にわたす

	uint16 RealUnderHP = 0;//HP処理関数の返り値,個体値0
	uint16 RealUpperHP = 0;//HP処理関数の返り値,個体値31
	bool HPmiri = false;//false=まんたん,true=HPのこりいち
	float BoalRATE = 1.0F;//
	float StatusVL = 1.0F;

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

	const Font font{ 25 };//計算結果描画
	float unRs = 0;//HP個体値0の時の捕獲確率(100分率表記済み)
	float uPrs = 0;//HP個体値31のときの捕獲確率(percentage)


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
		font(U"ポケモン捕獲率計算ver0.01").draw(40, 20);
		font(U"推定捕獲率").draw(220, 80);
		font(U"{:.4f}%\n     ～{:.4f}%"_fmt(unRs, uPrs)).draw(210, 120);//小数点以下4桁まで表示する


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
				HPmiri = true;
			}
			else if (index0 == 0) {
				HPmiri = false;
			}
		}

		SimpleGUI::Headline(U"使うボール", Vec2{ 200, 230 });
		if (SimpleGUI::RadioButtons(index1, BOAL, Vec2{ 200, 270 })) {
			switch (index1) {
			case 0://モンスターボール
				BoalRATE = 1.0F;
				break;
			case 1://スーパーボール
				BoalRATE = 1.5F;
				break;
			case 2://ハイパーボール
				BoalRATE = 2.0F;
				break;
			case 3://クイックボール
				BoalRATE = 5.0F;
				break;
			case 4://ダークボール
				BoalRATE = 3.0F;
				break;
			case 5://リピートボール
				BoalRATE = 3.0F;
				break;
			case 6:
				BoalRATE = 4.0F;
				break;
			}
		}

		SimpleGUI::Headline(U"状態異常", Vec2{ 20, 390 });
		if (SimpleGUI::RadioButtons(index2, Sick, Vec2{ 20, 430 })) {
			switch (index2) {
			case 0:
				StatusVL = 1.0F;
				break;
			case 1:
				StatusVL = 1.5F;
				break;
			case 2:
				StatusVL = 2.5F;
				break;
			}
		}



		if (NameTable.contains(tex01.text) && Level > 0 && Level <= 100) {
			//入力名が有効かつLevelが有効な数字であるなら
			KeisanCan = true;//捕獲率計算ボタンを有効にする		
		}
		else {
			KeisanCan = false;//そうでないならば計算させない
		}

		if (SimpleGUI::Button(U"捕獲率を計算する", Vec2{ 200,555 }, unspecified, KeisanCan)) {
			RealUnderHP = Pokemon_ExistLV_HP(PokeValue[NameTable[tex01.text]].SyuzokuHP, Level, 0);
			RealUpperHP = Pokemon_ExistLV_HP(PokeValue[NameTable[tex01.text]].SyuzokuHP, Level, 31);

			unRs = Percentage(PCMTHOD(RealUnderHP, HPmiri, PokeValue[NameTable[tex01.text]].CatchRATE, BoalRATE, StatusVL), 255.0F);
			uPrs = Percentage(PCMTHOD(RealUpperHP, HPmiri, PokeValue[NameTable[tex01.text]].CatchRATE, BoalRATE, StatusVL), 255.0F);

		}
		// "Licenses" ボタンが押されたら
		if (SimpleGUI::Button(U"Licenses", Vec2{ 20, 555 }))
		{
			// ライセンス情報を表示
			LicenseManager::ShowInBrowser();
		}

	}
}
