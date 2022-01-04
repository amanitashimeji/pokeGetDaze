# include <Siv3D.hpp> // OpenSiv3D v0.6.3

namespace /*構造体*/ {

	struct Stats {
		uint32 base_hp;
		uint32 catch_rate;
	};

	struct Ball {

		uint32 id;
		uint32 type;
		double default_rate;
		double max_rate;

	};

	struct Ailment {
		String name;
		double rate;
	};

};


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
					.base_hp = Parse<uint32>(csv[i][2]),
					.catch_rate = Parse<uint32>(csv[i][3]),
				};

				data_ << kari;

			}
			is_valid_ = true;
		}

	}

	bool isValid() const
	{
		return is_valid_;
	}

	bool isContains(const String& pokemon_name) const {

		return name_list_.contains(pokemon_name);
	}

	uint32 GetIDfromName(const String& pokemon_name) {
		return  name_list_[pokemon_name];
	}

	uint32 GetCatchRateFromID(const uint32& ID) {
		return data_[ID].catch_rate;

	}

	uint32 GetCatchRateFromName(const String& pokemon_name) {
		return data_[name_list_[pokemon_name]].catch_rate;
	}

	std::pair<uint32,uint32> RangeOfRealHP(const String& pokemon_name,const uint32 &level){

		constexpr uint32 kIndividual_ZERO = 0;
		constexpr uint32 kIndividual_V = 31;

		const uint32 ID=GetIDfromName(pokemon_name);

		const uint32 hp_lower_limit = CalculateHPfromLV(data_[ID].base_hp, level, kIndividual_ZERO);
		const uint32 hp_upper_limit = CalculateHPfromLV(data_[ID].base_hp, level, kIndividual_V);

		return { hp_lower_limit , hp_upper_limit };
		
	}

private:
	uint32 CalculateHPfromLV(const uint32& base_hp, const uint32& level, const uint32& individual_hp) {

		return ((((base_hp * 2) + individual_hp) * level / 100) + (level + 10));

	}

	HashTable<String, uint32> name_list_;

	Array<Stats> data_;

	bool is_valid_ = false;

};

class PokeBalls {
public:
	PokeBalls() = default;
	explicit PokeBalls(const FilePath& csvPath){

		const CSV csv{ csvPath };
		if (csv) {

			Ball kari ={
				.id = 0,
				.type = 0,
				.default_rate = 0,
				.max_rate = 0,
			};
			balls_data_ << kari;

			for (size_t row = 1; row < csv.rows(); ++row) {
				name_list_.emplace(csv[row][1], Parse<uint32>(csv[row][0]));

				kari = {

				.id = Parse<uint32>(csv[row][0]),
				.type = Parse<uint32>(csv[row][3]),
				.default_rate = Parse<double>(csv[row][4]),
				.max_rate = Parse<double>(csv[row][5]),

				};

				balls_data_ << kari;

			}

			is_valid_ = true;

		}

	}

	bool isValid()const{

		return is_valid_;

	}

	double BallRate(const String& ball_name) {
		uint32 ball_id = name_list_[ball_name];
		return balls_data_[ball_id].max_rate;

	}

private:

	HashTable<String, uint32>name_list_;

	Array<Ball> balls_data_;

	bool is_valid_ = false;

};

class BallsNameList {
public:

	explicit BallsNameList()
	:active_list_is_(1){};

	bool activeButtonNumber(const uint8 button_number) {
		if (active_list_is_ == button_number) {
			return false;
		}
		else {
			return true;
		}
	}

	Array<String> UpdateActiveList(const uint8 button_number) {

		active_list_is_ = button_number;

		switch (active_list_is_) {
		case 1:		return first_list_;		break;
		case 2:		return second_list_;	break;
		case 3:		return third_list_;		break;
		}

	}

	String SelectedBallName(const size_t& balls_index)const {
		switch (active_list_is_) {
		case 1: return first_list_[balls_index];  break;
		case 2: return second_list_[balls_index]; break;
		case 3: return third_list_[balls_index]; break;
		}


	}

private:

	uint8 active_list_is_;

	const Array<String> first_list_ = {

	U"モンスターボール",
	U"スーパーボール",
	U"ハイパーボール",
	U"リピートボール",
	U"ネットボール",
	U"タイマーボール",
	U"クイックボール",
	U"ラブラブボール",
	U"マスターボール",

	};

	const Array<String> second_list_ = {

		U"ダークボール",
		U"ダイブボール",
		U"ムーンボール",
		U"スピードボール",
		U"ネストボール",
		U"ドリームボール",
		U"ルアーボール",
		U"レベルボール",
		U"ウルトラボール",

	};

	const Array<String> third_list_ = {
		U"プレミアボール",
		U"ゴージャスボール",
		U"フレンドボール",
		U"ヒールボール",
		U"プレシャスボール",
		U"ヘビーボール",
		U"コンペボール",
		U"サファリボール",
		U"パークボール",
	};

};

class Ailments {
public:
	explicit Ailments() {};

	Array<String> NameList() {
		Array<String> name_list;
		for (const auto& [ailment, rate] : ailments_) {

			name_list << ailment;

		}
		return name_list;
	};

	double AilmentRate(const size_t &id)const {

		return ailments_[id].rate;

	}

private:

	const Array<Ailment> ailments_={

		{U"けんこう"	,1.0},
		{U"まひ"		,1.5},
		{U"やけど"		,1.5},
		{U"どく"		,1.5},
		{U"ねむり"		,2.5},
		{U"こおり"		,2.5},

	};

};

struct Result {

	uint32 lower_limit_hp_;
	uint32 upper_limit_hp_;
	uint32 catch_rate_;
	double rate_to_reduce_hp_;
	double ball_rate_;
	double ailment_rate_;

	std::pair<double, double> PokemonCaputureSuccessfulRange(){

		const uint32 lower_min_hp = MinHPCaluculate(lower_limit_hp_, rate_to_reduce_hp_);
		const uint32 upper_min_hp = MinHPCaluculate(upper_limit_hp_, rate_to_reduce_hp_);

		double result_lower_limit_rate = PokemonCaptureCaluculation(lower_limit_hp_, lower_min_hp);
		double result_upper_limit_rate = PokemonCaptureCaluculation(upper_limit_hp_, upper_min_hp);

		RepairToPercentage(result_lower_limit_rate);
		RepairToPercentage(result_upper_limit_rate);

		return {result_lower_limit_rate,result_upper_limit_rate};
		
	}

	String ResultString(){
		String result_string =
				U"実HP(個体値0):{}\n"_fmt(lower_limit_hp_)
			+	U"実HP(個体値31):{}\n"_fmt(upper_limit_hp_)
			+	U"捕まえやすさ:{}\n"_fmt(catch_rate_)
			+	U"ボール補正(最大値):{:.1f}\n"_fmt(ball_rate_)
			+	U"状態異常補正:{:.1f}\n"_fmt(ailment_rate_);

		return result_string;
		
	}

private:
	uint32 MinHPCaluculate(const uint32& full_of_hp, const double& rate_to_reduce_hp)const {

		constexpr uint32 kMinHPLowerLimit_Is_ONE = 1;

		const double temporary_min_hp = static_cast<double>(full_of_hp) * (rate_to_reduce_hp / 100);

		const uint32 result = Max(static_cast<uint32>(temporary_min_hp), kMinHPLowerLimit_Is_ONE);

		return result;
	}

	double PokemonCaptureCaluculation(const uint32& max_hp, const uint32& min_hp)const {

		return  (((max_hp * 3) - min_hp * 2) * catch_rate_ * ball_rate_ / (max_hp * 3)) * ailment_rate_;

	}

	void RepairToPercentage(double& dividend_number){
		constexpr double k_ONE_HUNDRED = 100;
		constexpr double k_Divisor_Number = 255.0;

		dividend_number= dividend_number / k_Divisor_Number * k_ONE_HUNDRED;

		return;
	}

};


void Main()
{
	const String VersionInfo = U"ポケモン捕獲率計算ver.0.03";
	Window::SetTitle(VersionInfo);
	//Window::Resize(420, 600);
	Pokedex pokedex(U"./PokemonData.csv");
	PokeBalls pokeballs(U"./BallsData.csv");	//TODO:これは中にぶちこんでアセット化したい
	BallsNameList ballsnames;
	Ailments ailments;

	Array<String> ailments_name_list=ailments.NameList();


	constexpr uint8 button_number_ONE = 1;
	constexpr uint8 button_number_TWO = 2;
	constexpr uint8 button_number_THREE = 3;

	std::variant<Array<String>> balls_list = ballsnames.UpdateActiveList(button_number_ONE);

	TextEditState name_box;		//ユーザー入力ポケモン名	name_box.text;
	TextEditState level_box;	//ユーザー入力レベル		Parse<uint32>(level_box.text);


	uint32 level = 0;			
	double slider_of_HP = 100.0;
	size_t balls_index = 0;
	size_t ailments_index = 0;


	bool avtivateNextTextBox = false;
	bool isUserInputValid = false;

	const Font font{ 25 ,Typeface::CJK_Regular_JP };//計算結果描画

	double result_lower_limit = 0;
	double result_upper_limit = 0;

	Result result = {

		.lower_limit_hp_	= 0,
		.upper_limit_hp_	= 0,
		.catch_rate_		= 0,
		.rate_to_reduce_hp_ = 0,
		.ball_rate_			= 0,
		.ailment_rate_		= 0,

	};

	String result_strings = U"";

	while (System::Update()) {
		ClearPrint();

		// フォント描画系

		font(VersionInfo).draw(40, 20);
		font(U"推定捕獲率\n(HP個体値0～HP個体値31)").draw(440, 80);
		font(U"{:.4f}% ～{:.4f}%"_fmt(result_lower_limit, result_upper_limit)).draw(440, 150);

		if (not result_strings.isEmpty()) {
			font(result_strings).draw(440, 200);
		}


		//小数点以下4桁まで表示する

		//フォント描画系終了


		//GUI:テキストボックス系

		if (avtivateNextTextBox)
		{
			// テキストボックスをアクティブ化
			level_box.active = true;

			avtivateNextTextBox = false;

		}

		const bool previous = name_box.active;
		SimpleGUI::Headline(U"ポケモン名", Vec2{ 20, 80 });
		SimpleGUI::TextBox(name_box, Vec2{ 18, 115 }, 160);

		if (previous && (name_box.active == false))
		{
			// Tab キーが入力されていた場合、次のテキストボックスをアクティブ化するフラグを true に
			avtivateNextTextBox = TextInput::GetRawInput().includes(U'\t');

		}

		SimpleGUI::Headline(U"レベル", Vec2{ 20, 160 }, 80);
		SimpleGUI::TextBox(level_box, Vec2{ 18, 195 }, 160);
		level = ParseOr<uint32>(level_box.text, 0);

		//GUI:テキストボックス系処理終了
		
		//GUI:HPスライダー
		SimpleGUI::Headline(U"残りHP", Vec2{ 20, 240 });
		SimpleGUI::Headline(U"{:.1f}%"_fmt(slider_of_HP), Vec2{ 100,240 },80);
		SimpleGUI::Slider(slider_of_HP, 0.1, 100.0, Vec2{20,275},160);

		//GUI:HPスライダー終了


		//GUI:選択肢ラジオボタン系処理

		if (SimpleGUI::Button(U"1", Vec2{ 290,195 },30,(ballsnames.activeButtonNumber(button_number_ONE)))) {

			balls_list = ballsnames.UpdateActiveList(button_number_ONE);

		}

		if (SimpleGUI::Button(U"2", Vec2{ 330,195 },30,(ballsnames.activeButtonNumber(button_number_TWO)))) {
			balls_list = ballsnames.UpdateActiveList(button_number_TWO);

		}

		if (SimpleGUI::Button(U"3", Vec2{ 370,195 },30,(ballsnames.activeButtonNumber(button_number_THREE)))) {
			balls_list = ballsnames.UpdateActiveList(button_number_THREE);

		}
		SimpleGUI::Headline(U"ボール", Vec2{ 200, 195 });
		SimpleGUI::RadioButtons(balls_index, std::get<Array<String>>(balls_list), Vec2{ 200, 235 }, 200);




		SimpleGUI::Headline(U"状態異常", Vec2{ 20, 320 });
		SimpleGUI::RadioButtons(ailments_index, ailments_name_list, Vec2{ 20, 355 }, 160);

		//GUI:選択肢系処理終了

		if (pokedex.isContains(name_box.text) && level > 0 && level <= 100) {
			//入力名が有効かつLevelが有効な数字であるなら
			isUserInputValid = true;//捕獲率計算ボタンを有効にする		
		}
		else {
			isUserInputValid = false;
		}


		if (SimpleGUI::Button(U"捕獲率を計算する", Vec2{ 200,115 }, unspecified, isUserInputValid)) {

			auto [lower_limit_HP,upper_limit_HP]=pokedex.RangeOfRealHP(name_box.text,level);

			result = {

				.lower_limit_hp_	= lower_limit_HP,
				.upper_limit_hp_	= upper_limit_HP,
				.catch_rate_		= pokedex.GetCatchRateFromName(name_box.text),
				.rate_to_reduce_hp_ = slider_of_HP,
				.ball_rate_			= pokeballs.BallRate(std::get<Array<String>>(balls_list)[balls_index]),
				.ailment_rate_		= ailments.AilmentRate(ailments_index),

			};

			auto[first,second]=result.PokemonCaputureSuccessfulRange();
			result_lower_limit = first;
			result_upper_limit = second;

			result_strings = result.ResultString();

		}

		//GUI:計算ボタン処理終了



		// "Licenses"
		if (SimpleGUI::Button(U"Licenses", Vec2{ 650, 555 }))
		{
			// ライセンス情報を表示
			LicenseManager::ShowInBrowser();
		}


	}
	
}
