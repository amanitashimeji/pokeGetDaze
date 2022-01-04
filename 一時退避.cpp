# include <Siv3D.hpp> // OpenSiv3D v0.6.3



namespace /*TODO */ {

	// もしかして　「ピカチュウ」
	//

	//HP残量→シークバー



};


void ReadPokemonDataFromCSV(HashTable<String, uint32>& PokemonNameList,
								Array<Stats>& PokemonData);
uint32 CalculateHPfromLV(const uint32& SHP, const uint32& LV, const uint32& KoHP);
float PokemonCaptureMethod(const ActArgumentList& a);

template<typename T>
T Percentage(T a, T b);





namespace /*構造体*/ {

	struct Stats {
		uint32 base_hp;
		uint32 catch_rate;
	};

	struct ActArgumentList {
		uint32 MaxHP = 1;
		uint32 CatchRATE = 3;
		float BollRATE = 1.0;	//x
		float StatusRATE = 1.0; //x
		bool HPFlag = false;

		//
	};


};



void ReadPokemonDataFromCSV(HashTable<String, uint32>& PokemonNameList,
								Array<Stats>& PokemonData)
{
	const CSV csv{ U"./PokemonData.csv" };//csv読み込み処理

	if (not csv) {
		throw Error{ U"Failed to load `PokemonDate.csv`" };//読み込めなかった場合のエラー処理
	}


	for (auto i : step(csv.rows())) {//csv→Hash,Array

		//行サイズをcsv.rows()で得ているので
		//形式に沿ってPokemonDate.csvに項目を追加すれば,exeを変えること無く最新データに対応可

		//NameTable.contains(U---);完全一致するテキストの検索;
		//NameTable[U--];一致テキストのキー(今回は図鑑No兼行番号);
		PokemonNameList.emplace(csv[i][1], i);

		Statistics kari = {

			.SyuzokuHP = Parse<uint32>(csv[i][2]),
			.CatchRATE = Parse<uint32>(csv[i][3]),

		};

		PokemonData << kari;
	}
}

uint32 CalculateHPfromLV(const uint32& SHP, const uint32& LV, const uint32& KoHP) {

	return ((((SHP * 2) + KoHP) * LV / 100) + (LV + 10));

}

float PokemonCaptureMethod(const ActArgumentList& a) {
	uint32 MinHP = a.MaxHP * 2;
	if (a.HPFlag) {
		MinHP = 2;
	}
	return (((a.MaxHP * 3) - MinHP) * a.CatchRATE * a.BollRATE / (a.MaxHP * 3)) * a.StatusRATE;

}

template<typename T>
T Percentage(T a, T b)//100分率表記になおす
{
	return a / b * 100;
}


class Pokedex
{
public:

	Pokedex() = default;

	explicit Pokedex(const FilePath& csvPath)
	{
		const CSV csv{ csvPath };
		if (csv)
		{
			for (auto i : step(csv.rows())) {
				name_list_.emplace(csv[i][1], i);

				Stats kari = {
					.base_HP = Parse<uint32>(csv[i][2]),
					.catch_rate = Parse<uint32>(csv[i][3]),
				};

				data_ << kari;

			}
			is_loaded_ = true;
		}


	}
	bool isLoaded() const
	{
		return true;
	}

private:
	HashTable<String, uint32> name_list_;
	Array<Stats> data_;
	bool is_loaded_ = false;

};



void Main()

{

	const String VersionInfo = U"ポケモン捕獲率計算ver.0.02";
	Window::SetTitle(VersionInfo);
	Window::Resize(420, 600);

	Pokedex PokeDex(U"./PokemonData.csv");



	HashTable<String, uint32>PokemonNameList;	//	<ポケモン名,図鑑No>
	Array<Stats>PokemonData;					//	PokemonDate[i].HP PokemonDate[i].CR

	ReadPokemonDataFromCSV(PokemonNameList, PokemonData);

	const Array<String>HPdata = { U"まんたん",U"のこり1" };//選択肢配列
	const Array<String>Bolls = {
		U"モンスターボール",
		U"スーパーボール",
		U"ハイパーボール",
		U"クイックボール",
		U"ダークボール",
		U"リピートボール",
		U"タイマーボール",
	};
	const Array<String>Ailments = { U"なにもしない",U"まひ",U"ねむり" };

	TextEditState NameTextBox;//ユーザー入力ポケモン名
	TextEditState LevelTextBox;//ユーザー入力レベル
	uint32 level = 0;//tex02.text(Parse<uint16>),実HP処理関数にわたす
	size_t HPdatasIndex = 0;//残りHP選択肢
	size_t BollsIndex = 0;//使うボール選択肢
	size_t AlimentsIndex = 0;//状態異常



	bool avtivateNextTextBox = false;

	bool isUserInputValid = false;//入力値を見て計算可能か判断するフラグ

	const Font font{ 25 ,Typeface::CJK_Regular_JP };//計算結果描画
	float result_lower_limit = 0;//HP個体値0の時の捕獲確率
	float result_upper_limit = 0;//HP個体値31のときの捕獲確率

	ActArgumentList PokemonArgment;

	while (System::Update())
	{
		ClearPrint();


		//Print << Cursor::Pos();//GUI配置のためのマウスカーソル座標

		// フォント描画系

		font(VersionInfo).draw(40, 20);
		font(U"推定捕獲率").draw(220, 80);
		font(U"{:.4f}%\n     ～{:.4f}%"_fmt(result_lower_limit, result_upper_limit)).draw(210, 120);
		//小数点以下4桁まで表示する

		//フォント描画系終了


		//GUI:テキストボックス系処理

		if (avtivateNextTextBox)
		{
			// テキストボックスをアクティブ化
			LevelTextBox.active = true;

			avtivateNextTextBox = false;

		}

		const bool previous = NameTextBox.active;
		SimpleGUI::Headline(U"ポケモン名", Vec2{ 20, 80 });
		SimpleGUI::TextBox(NameTextBox, Vec2{ 18, 115 }, 130);

		if (previous && (NameTextBox.active == false))
		{
			// Tab キーが入力されていた場合、次のテキストボックスをアクティブ化するフラグを true に
			avtivateNextTextBox = TextInput::GetRawInput().includes(U'\t');

		}

		SimpleGUI::Headline(U"レベル", Vec2{ 20, 160 }, 80);
		SimpleGUI::TextBox(LevelTextBox, Vec2{ 18, 195 }, 100);
		level = ParseOr<uint32>(LevelTextBox.text, 0);

		//GUI:テキストボックス系処理終了


		//GUI:選択肢ラジオボタン系処理

		SimpleGUI::Headline(U"残りHP", Vec2{ 20, 260 });
		if (SimpleGUI::RadioButtons(HPdatasIndex, HPdata, Vec2{ 20, 300 })) {
			if (HPdatasIndex == 1) {
				PokemonArgment.HPFlag = true;
			}
			else if (HPdatasIndex == 0) {
				PokemonArgment.HPFlag = false;
			}
		}

		SimpleGUI::Headline(U"使うボール", Vec2{ 200, 230 });
		if (SimpleGUI::RadioButtons(BollsIndex, Bolls, Vec2{ 200, 270 })) {
			switch (BollsIndex) {
			case 0://モンスターボール
				PokemonArgment.BollRATE = 1.0F;
				break;
			case 1://スーパーボール
				PokemonArgment.BollRATE = 1.5F;
				break;
			case 2://ハイパーボール
				PokemonArgment.BollRATE = 2.0F;
				break;
			case 3://クイックボール
				PokemonArgment.BollRATE = 5.0F;
				break;
			case 4://ダークボール
			case 5://リピートボール //faulthrough
				PokemonArgment.BollRATE = 3.0F;
				break;
			case 6://タイマーボール10T以降
				PokemonArgment.BollRATE = 4.0F;
				break;
			}
		}

		SimpleGUI::Headline(U"状態異常", Vec2{ 20, 390 });
		if (SimpleGUI::RadioButtons(AlimentsIndex, Ailments, Vec2{ 20, 430 })) {
			switch (AlimentsIndex) {
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

		//GUI:選択肢系処理終了


		//GUI:計算ボタン

		if (PokemonNameList.contains(NameTextBox.text) && level > 0 && level <= 100) {
			//入力名が有効かつLevelが有効な数字であるなら
			isUserInputValid = true;//捕獲率計算ボタンを有効にする		
		}
		else {
			isUserInputValid = false;
		}


		if (SimpleGUI::Button(U"捕獲率を計算する", Vec2{ 200,555 }, unspecified, isUserInputValid)) {
			PokemonArgment.CatchRATE = PokemonData[PokemonNameList[NameTextBox.text]].catch_rate;

			//HP種族値0の時
			PokemonArgment.MaxHP = CalculateHPfromLV(PokemonData[PokemonNameList[NameTextBox.text]].base_hp, level, 0);
			result_lower_limit = Percentage(PokemonCaptureMethod(PokemonArgment), 255.0F);


			//HP種族値31の時
			PokemonArgment.MaxHP = CalculateHPfromLV(PokemonData[PokemonNameList[NameTextBox.text]].base_hp, level, 31);
			result_upper_limit = Percentage(PokemonCaptureMethod(PokemonArgment), 255.0F);

		}

		//GUI:計算ボタン処理終了



		// "Licenses"
		if (SimpleGUI::Button(U"Licenses", Vec2{ 20, 555 }))
		{
			// ライセンス情報を表示
			LicenseManager::ShowInBrowser();
		}

	}
}
