#include <Siv3D.hpp> // OpenSiv3D v0.4.1
#include <bits/stdc++.h>
using namespace std;

struct Rocket {
	double x, y, degree;
};


struct Brock {
	double x, y, degree;
	double speed, size;
	int H;
	int near;
};

//ファイル入出力
ifstream fin;
ofstream fout;
void wait(int ms) {
	clock_t st = clock();
	while ((clock() - st) * 1000 / CLOCKS_PER_SEC <= ms);
}
void Main() {
	srand((unsigned)(time(NULL)));
	Window::Resize(1200, 600);
	Window::SetTitle(U"Space Run2");
	Window::Centering();

	//シチュエーション管理
	int now_situation = 0;
	int next_situation = 1;

	//難易度管理
	int difficulty = 1;

	//フォント
	Font font_120_italy(120, Typeface::Regular, FontStyle::Italic);
	Font font_75_italy(75, Typeface::Regular, FontStyle::Italic);
	Font font_35_italy(35, Typeface::Regular, FontStyle::Italic);
	Font font_25_italy(25, Typeface::Regular, FontStyle::Italic);
	Font font_15_italy(15, Typeface::Regular, FontStyle::Italic);
	//テクスチャ
	Texture rocket_texture(U"GameData/rocket.png", TextureDesc::Mipped);
	Rocket rocket;
	rocket.x = 600., rocket.y = 450, rocket.degree = 0.;
	Texture twitter_rink(U"GameData/twitter.png");
	Texture space(U"GameData/space.png");
	//弾
	vector<Brock>brock;
	//タイム管理
	Stopwatch gaming_time;
	int game_stop = 0;
	//FPS上限
	Graphics::SetTargetFrameRateHz(60);
	//弾の色管理
	int red_brock = -1;
	//スコア
	int game_hp = 100, near_score = 0;
	//順位表
	vector<int>scores[10];
	bool get_scores = false;
	bool write_scores = false;

	//音楽
	const Audio opening(U"GameData/opening.mp3", Arg::loop = true);
	const Audio kettei(U"GameData/kettei.mp3");
	const Audio sys(U"GameData/sys.mp3");
	const Audio tutorial(U"GameData/tutorial.mp3", Arg::loop = true);
	const Audio cancel(U"GameData/cancel.mp3");
	const Audio author(U"GameData/author.mp3", Arg::loop = true);
	const Audio level(U"GameData/level.mp3", Arg::loop = true);
	const Audio gameplay(U"GameData/gameplay.mp3");
	const Audio bomb(U"GameData/bomb.mp3");
	while (System::Update()) {
		space.draw();
		//start画面
		if (now_situation == 0) {
			//順位表を取得
			opening.play();
			if (!get_scores) {
				fin.open("GameData/Scores.txt", ios::in);
				while (!fin.eof()) {
					string sc;
					getline(fin, sc);
					if (sc == "")break;
					int newsc = stoi(sc);
					scores[newsc / 10000000].push_back(newsc % 10000000);
				}
				fin.close();
				get_scores = true;
				for (int i = 0; i < 10; i++) {
					sort(scores[i].begin(), scores[i].end(), greater<>());
				}
			}

			//表示
			String title = U"SpaceRun2";
			for (int i = 0; i < title.size(); i++) {
				font_120_italy(title[i]).draw(280 + i * 70, 60, HSV(i * 36));
			}
			Rect(420, 270, 360, 60).shearedX(120).draw(Palette::Turquoise);
			Rect(420, 360, 360, 60).shearedX(120).draw(Palette::Turquoise);
			Rect(420, 450, 360, 60).shearedX(120).draw(Palette::Turquoise);
			font_35_italy(U"チュートリアル").drawAt(600, 300);
			font_35_italy(U"プレイ").drawAt(600, 390);
			font_35_italy(U"作者").drawAt(600, 480);
			//font_25_italy(U"...はじめてプレイする方向け").draw(750, 270);
			//font_25_italy(U"...ゲームをプレイ").draw(750, 360);
			//font_25_italy(U"...このゲームを作った人").draw(750, 450);
			if (Scene::FrameCount() % 60 < 30)font_25_italy(U"↑↓キーで選択、Enterキーで決定").drawAt(600, 570);
			if (KeyDown.down()) {
				next_situation = min(3, next_situation + 1);
				sys.play();
			}
			if (KeyUp.down()) {
				next_situation = max(1, next_situation - 1);
				sys.play();
			}
			Rect(420, 180 + next_situation * 90, 360, 60).shearedX(120).drawFrame(7, Palette::Yellow);
			if (KeyEnter.down()) {
				now_situation = next_situation;
				next_situation = 1;
				opening.stop();
				kettei.play();
				wait(500);
			}
		}
		else if (now_situation == 1) {
			tutorial.play();
			//文字の描画
			font_75_italy(U"チュートリアル").drawAt(600, 120, Palette::Deepskyblue);
			font_25_italy(U"降ってくるカラフルな隕石を避けましょう！").drawAt(600, 225);
			font_25_italy(U"隕石に近ければ近いほどスコアが高くなります").drawAt(600, 265);
			font_25_italy(U"操作方法：マウスを動かしてロケットを動かす").drawAt(600, 300);
			font_25_italy(U"ロケットはカーソルのある方向に飛んでいきます。").drawAt(600, 340);
			font_25_italy(U"練習してみましょう！").drawAt(600, 375);
			if (Scene::FrameCount() % 60 < 30)font_25_italy(U"Enterキーでホーム画面に戻る").drawAt(600, 570);
			//ロケットの描画
			rocket_texture.scaled(0.5 * 0.75).rotatedAt(rocket_texture.width() / 4 * 0.75, rocket_texture.height() / 4 * 0.75, ToRadians(rocket.degree)).drawAt(rocket.x, rocket.y);

			//ロケットの移動

			if (abs(rocket.x - Cursor::Pos().x) <= 15 && abs(rocket.y - Cursor::Pos().y) <= 15)goto loop;
			rocket.x += cos(ToRadians(90 - rocket.degree)) * Scene::DeltaTime() * 600 * 0.75;
			rocket.x = max(rocket.x, 100.);
			rocket.x = min(rocket.x, 1100.);
			rocket.y -= sin(ToRadians(90 - rocket.degree)) * Scene::DeltaTime() * 600 * 0.75;
			rocket.y = max(rocket.y, 100.);
			rocket.y = min(rocket.y, 500.);
		loop:;
			rocket.degree = 90 - ToDegrees(atan2(rocket.y - Cursor::Pos().y, Cursor::Pos().x - rocket.x));
			if (KeyEnter.down()) {
				now_situation = 0;
				rocket.x = 600., rocket.y = 450, rocket.degree = 0.;
				tutorial.stop();
				cancel.play();
				wait(500);
			}
		}
		else if (now_situation == 2) {
			level.play();
			//文字・選択バーの描画
			font_75_italy(U"難易度選択").drawAt(800 * 0.75, 160 * 0.75, Palette::Deepskyblue);
			Rect(240 * 0.75, 320 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 400 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 480 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 560 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 640 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			font_25_italy(U"レベル１").drawAt(480 * 0.75, 340 * 0.75);
			font_25_italy(U"レベル２").drawAt(480 * 0.75, 420 * 0.75);
			font_25_italy(U"レベル３").drawAt(480 * 0.75, 500 * 0.75);
			font_25_italy(U"レベル４").drawAt(480 * 0.75, 580 * 0.75);
			font_25_italy(U"レベル５").drawAt(480 * 0.75, 660 * 0.75);
			if (scores[1].size())font_25_italy(U"HighScore: ", scores[1][0]).draw(1000 * 0.75, 320 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 320 * 0.75);
			if (scores[2].size())font_25_italy(U"HighScore: ", scores[2][0]).draw(1000 * 0.75, 400 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 400 * 0.75);
			if (scores[3].size())font_25_italy(U"HighScore: ", scores[3][0]).draw(1000 * 0.75, 480 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 480 * 0.75);
			if (scores[4].size())font_25_italy(U"HighScore: ", scores[4][0]).draw(1000 * 0.75, 560 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 560 * 0.75);
			if (scores[5].size())font_25_italy(U"HighScore: ", scores[5][0]).draw(1000 * 0.75, 640 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 640 * 0.75);
			if (Scene::FrameCount() % 60 < 30)font_25_italy(U"数字キーを使って選ぶこともできます。").drawAt(800 * 0.75, 760 * 0.75);

			//選択
			if (KeyDown.down()) {
				difficulty = min(5, difficulty + 1);
				sys.play();
			}
			if (KeyUp.down()) {
				difficulty = max(1, difficulty - 1);
				sys.play();
			}
			Rect(240 * 0.75, 240 * 0.75 + difficulty * 80 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).drawFrame(10 * 0.75, Palette::Yellow);
			if (KeyEnter.down()) {
				now_situation = 4; kettei.play(); level.stop();
			}
			//コマンド
			if ((Key1 | KeyNum1).down()) {
				difficulty = 1; sys.play();
			}
			if ((Key2 | KeyNum2).down()) {
				difficulty = 2; sys.play();
			}
			if ((Key3 | KeyNum3).down()) {
				difficulty = 3; sys.play();
			}
			if ((Key4 | KeyNum4).down()) {
				difficulty = 4; sys.play();
			}
			if ((Key5 | KeyNum5).down()) {
				difficulty = 5; sys.play();
			}
		}
		else if (now_situation == 3) {
			author.play();
			font_75_italy(U"作者").drawAt(600, 120, Palette::Deepskyblue);
			font_35_italy(U"原案・デザイン・プログラム：define").drawAt(600, 300);
			Rect twitter_click(800 * 0.75 - twitter_rink.width() / 8 * 0.75, 450 * 0.75, twitter_rink.width() / 4 * 0.75, twitter_rink.height() / 4 * 0.75);
			twitter_rink.scaled(0.25 * 0.75).draw(800 * 0.75 - twitter_rink.width() / 8 * 0.75, 450 * 0.75);
			font_35_italy(U"音楽：魔王魂").drawAt(600, 400);
			if (twitter_click.leftClicked()) {
				System::LaunchBrowser(U"https://twitter.com/define_AC");
			}
			if (Scene::FrameCount() % 60 < 30)font_25_italy(U"Enterキーでホーム画面に戻る").drawAt(800 * 0.75, 750 * 0.75);
			if (KeyEnter.down()) {
				now_situation = 0;
				author.stop();
				cancel.play();
			}
		}
		else if (now_situation == 4) {
			//デバッグ用コマンド
			//if (KeyE.down())now_situation = 5;

			Triangle rocket_upper(rocket.x, rocket.y - 54 * 0.75, rocket.x + 12 * 0.75, rocket.y + 16 * 0.75, rocket.x - 12 * 0.75, rocket.y + 16 * 0.75);
			Rect rocket_lower(rocket.x - 22 * 0.75, rocket.y + 16 * 0.75, 44 * 0.75, 39 * 0.75);
			//ロケットの描画
			rocket_texture.scaled(0.5 * 0.75).rotatedAt(rocket_texture.width() / 4 * 0.75, rocket_texture.height() / 4 * 0.75, ToRadians(rocket.degree)).drawAt(rocket.x, rocket.y);


			if (game_stop % 2 == 0) {
				if (MouseL.down()) {
					gaming_time.start();
					game_stop++;
					gameplay.play();
					continue;
				}
				for (auto& p : brock) {
					Circle brock_p(p.x, p.y, p.size);
					if (gaming_time.s() > red_brock)brock_p.draw(HSV(p.H));
					else brock_p.draw(Palette::Red);
				}
				if (game_stop)font_75_italy(U"クリックして再開").drawAt(800 * 0.75, 400 * 0.75);
				else {
					font_75_italy(U"クリックしてスタート").drawAt(800 * 0.75, 400 * 0.75);
					font_25_italy(U"途中でクリックすると一時停止します").drawAt(800 * 0.75, 500 * 0.75);
				}
				//スコアの表示
				Circle(40, 40, 40).drawPie(0, ToRadians(gaming_time.s() * 6), Palette::Red);
				font_25_italy(gaming_time.s()).drawAt(40, 40, Palette::Yellow);
				font_25_italy(U"Score:").draw(10, 110, Palette::Lightyellow);
				font_25_italy(near_score).draw(10, 135, Palette::Springgreen);
				//HPバー
				Rect(10, 180, 40, 400).drawFrame(0, 5, Palette::Yellowgreen);
				for (int i = 180 + (100 - game_hp) * 4; i < 580; i += 4) {
					if (game_hp)Rect(10, i, 40, 5).draw(HSV((i - 180) / (400. / 360.)));
				}
				continue;
			}
			else if (MouseL.down()) {
				game_stop++;
				gaming_time.pause();
				gameplay.pause();
				continue;
			}
			//ロケットの移動

			if (abs(rocket.x - Cursor::Pos().x) <= 50 && abs(rocket.y - Cursor::Pos().y) <= 50)goto loop3;
			rocket.x += cos(ToRadians(90 - rocket.degree)) * Scene::DeltaTime() * 60 * 4 * max(3, difficulty) * 0.75;
			rocket.x = max(rocket.x, 100.);
			rocket.x = min(rocket.x, 1100.);
			rocket.y -= sin(ToRadians(90 - rocket.degree)) * Scene::DeltaTime() * 60 * 4 * max(3, difficulty) * 0.75;
			rocket.y = max(rocket.y, 100.);
			rocket.y = min(rocket.y, 550.);
		loop3:;
			rocket.degree = 90 - ToDegrees(atan2(rocket.y - Cursor::Pos().y, Cursor::Pos().x - rocket.x));

			//ブロックの追加
			if (Scene::FrameCount() % (120 / (difficulty + 1)) == 0) {
				double start_x = rand() % 2000 - 400, end_x = rand() % 1200;
				double degree = 90 - ToDegrees(atan2(500, end_x - start_x));
				brock.push_back({ start_x,0.,degree,gaming_time.s() / 20 * difficulty * 0.75 + 2 * (difficulty + 1) * 0.75,gaming_time.s() / 10 * difficulty * 0.75 + 5,Random(0,360),10000 });
			}
			else if (Scene::FrameCount() % (120 / (difficulty + 1)) == 120 / (difficulty + 1) / 2) {
				double start_x = rand() % 2000 - 400, end_x = rocket.x + Random(-300, 300);
				double degree = 90 - ToDegrees(atan2(rocket.y, end_x - start_x));
				brock.push_back({ start_x,0.,degree,gaming_time.s() / 20 * difficulty * 0.75 + 2 * (difficulty + 1) * 0.75,gaming_time.s() / 10 * difficulty * 0.75 + 5,Random(0,360),10000 });
			}
			//ブロックの当たり判定
			for (int i = 0; i < brock.size(); i++) {
				Brock &p = brock[i];
				Circle brock_p(p.x, p.y, p.size);
				//ロケットの当たり判定
				if (rocket_upper.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)).intersects(brock_p) || rocket_lower.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)).intersects(brock_p)) {
					game_hp -= 5;
					brock[i] = brock.back();
					brock.pop_back(); i--;
					red_brock = gaming_time.s() + 1;
					bomb.stop(); bomb.play();
					continue;
				}
				else if (gaming_time.s() > red_brock) {
					p.near = min(p.near, (int)(Geometry2D::Distance(rocket_upper.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)), brock_p)));
					p.near = min(p.near, (int)(Geometry2D::Distance(rocket_lower.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)), brock_p)));
				}
				//範囲判定
				if (p.y > 600.) {
					if (p.near) {
						near_score += (200 / max(1, p.near)) * (200 / max(1, p.near));
					}
					brock[i] = brock.back(); brock.pop_back(); i--;
					continue;
				}
				if (gaming_time.s() > red_brock)brock_p.draw(HSV(p.H));
				else brock_p.draw(Palette::Red);
				//ブロックの移動
				p.x += cos(ToRadians(90 - p.degree)) * Scene::DeltaTime() * p.speed * 60;
				p.y += sin(ToRadians(90 - p.degree)) * Scene::DeltaTime() * p.speed * 60;
			}
			//HPの回復
			if (Scene::FrameCount() % 180 == 0)game_hp = min(game_hp + 1, 100);

			//スコアの表示
			Circle(40, 40, 40).drawPie(0, ToRadians(gaming_time.s() * 6), Palette::Red);
			font_25_italy(gaming_time.s()).drawAt(40, 40, Palette::Yellow);
			font_25_italy(U"Score:").draw(10, 110, Palette::Lightyellow);
			font_25_italy(near_score).draw(10, 135, Palette::Springgreen);
			//HPバー
			Rect(10, 180, 40, 400).drawFrame(0, 5, Palette::Yellowgreen);
			for (int i = 180 + (100 - game_hp) * 4; i < 580; i += 4) {
				if (game_hp)Rect(10, i, 40, 5).draw(HSV((i - 180) / (400. / 360.)));
			}
			//ゲーム終了
			if (game_hp <= 0 || gaming_time.s() >= 60) {
				for (auto& p : brock) {
					if (p.near) {
						near_score += (200 / max(1, p.near)) * (200 / max(1, p.near));
					}
				}
				now_situation = 5; gameplay.stop();
			}
		}
		else if (now_situation == 5) {
			gaming_time.pause();
			//スコア表示
			font_75_italy(U"結果").drawAt(800 * 0.75, 160 * 0.75, Palette::Deepskyblue);
			font_35_italy(U"   レベル：", difficulty, U"    タイム：", gaming_time.s(), U"秒", U"    スコア：", near_score).drawAt(600, 200, Palette::Lightgreen);

			//スコア書き込み
			if (!write_scores) {
				scores[difficulty].push_back(near_score);
				sort(scores[difficulty].begin(), scores[difficulty].end(), greater<>());
				fout.open("GameData/Scores.txt", ios::app);
				fout << 10000000 * difficulty + near_score << endl;
				fout.close();
				write_scores = true;
			}

			//順位表
			bool isused = false;
			for (int i = 0; i < 50; i++) {
				if (i < scores[difficulty].size()) {
					if (scores[difficulty][i] == near_score && !isused) {
						isused = true;
						if (Scene::FrameCount() % 60 < 30) {
							font_25_italy(i + 1, U"位   ", scores[difficulty][i]).draw(200 + i / 10 * 180, 250 + i % 10 * 28, HSV(i * 7.2));
						}
					}
					else {
						font_25_italy(i + 1, U"位   ", scores[difficulty][i]).draw(200 + i / 10 * 180, 250 + i % 10 * 28, HSV(i * 7.2));
					}
				}
				else {
					font_25_italy(i + 1, U"位   -").draw(200 + i / 10 * 180, 250 + i % 10 * 28, HSV(i * 7.2));
				}
			}

			if (Scene::FrameCount() % 60 < 30)font_25_italy(U"Enterキーでホーム画面に戻る").drawAt(800 * 0.75, 570);
			if (KeyEnter.down()) {
				//初期化
				now_situation = 0;
				next_situation = 1;
				near_score = 0;
				game_hp = 100;
				difficulty = 1;
				brock.clear();
				red_brock = -1;
				rocket.x = 600., rocket.y = 450, rocket.degree = 0.;
				game_stop = 0;
				gaming_time.reset();
				get_scores = false;
				write_scores = false;
				for (int i = 0; i < 10; i++)scores[i].clear();
				kettei.play();
			}
		}
	}
}