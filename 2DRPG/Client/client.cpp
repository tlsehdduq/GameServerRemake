#define SFML_STATIC 1
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <Windows.h>
#include <chrono>
using namespace std;

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#include"../Server/protocol.h"
sf::TcpSocket s_socket;

constexpr auto SCREEN_WIDTH = 10;
constexpr auto SCREEN_HEIGHT = 10;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = SCREEN_WIDTH * TILE_WIDTH;   // size of window
constexpr auto WINDOW_HEIGHT = SCREEN_WIDTH * TILE_WIDTH;

int g_left_x;
int g_top_y;
int g_myid;
char Name[NAME_SIZE];

////////////////////// wc //////////////////////////
bool g_isChat;
sf::Text chatmessage;
vector<sf::Text> curChatMessage;

void CreateChatMessage(string _message);
void SetCurMessage(string _message);
/// ////////////////////////////////////////////////

sf::RenderWindow* g_window;
sf::Font g_font;

class OBJECT {
public:
	bool m_isleft = false;
	bool m_isattack = false;
	bool m_showing;
	sf::Sprite m_sprite;

	sf::Text m_name;
	sf::Text m_chat;
	chrono::system_clock::time_point m_mess_end_time;

	int m_currentFrame;
	int m_frameCount;
	sf::Vector2i m_frameSize;
	sf::Clock m_animationClock;
	float m_animationSpeed;

public:
	int id;
	int m_x, m_y;
	char name[NAME_SIZE];
	bool mleft = false;
	OBJECT(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		set_name("NONAME");
		m_mess_end_time = chrono::system_clock::now();
	}
	OBJECT() {
		m_showing = false;
	}

	void updateAnimation() {
		if (m_animationClock.getElapsedTime().asSeconds() > m_animationSpeed) {
			m_currentFrame = (m_currentFrame + 1) % m_frameCount;
			m_sprite.setTextureRect(sf::IntRect(m_frameSize.x * m_currentFrame, 0, m_frameSize.x, m_frameSize.y));
			m_animationClock.restart();
		}
	}
	void attackAnimation() {

	}
	void show()
	{
		m_showing = true;
	}
	void hide()
	{
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}

	void a_draw() {
		g_window->draw(m_sprite);
	}

	void move(int x, int y) {
		m_x = x;
		m_y = y;
	}
	void draw() {
		if (false == m_showing) return;
		//updateAnimation();
		float rx = (m_x - g_left_x) * 65.0f + 1;
		float ry = (m_y - g_top_y) * 65.0f + 1;
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		auto size = m_name.getGlobalBounds();
		if (m_mess_end_time < chrono::system_clock::now()) {
			m_name.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_name);
		}
		else {
			m_chat.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_chat);
		}
	}
	void set_scale(float x, float y) {
		m_sprite.setScale(x, y);
	}
	void set_name(const char str[]) {
		m_name.setFont(g_font);
		m_name.setString(str);
		if (id < MAX_USER) m_name.setFillColor(sf::Color(255, 255, 255));
		else m_name.setFillColor(sf::Color(255, 255, 0));
		m_name.setStyle(sf::Text::Bold);
	}

	void set_chat(const char str[]) {
		m_chat.setFont(g_font);
		m_chat.setString(str);
		m_chat.setFillColor(sf::Color(255, 255, 255));
		m_chat.setStyle(sf::Text::Bold);
		m_mess_end_time = chrono::system_clock::now() + chrono::seconds(3);
	}
};

OBJECT avatar;
unordered_map <int, OBJECT> players;
unordered_map <int, OBJECT> monsters;
OBJECT white_tile;
OBJECT black_tile;
OBJECT Monser;

////////////////////// wc //////////////////////////
OBJECT chatUI;
sf::Texture* ChatUI;
/// ////////////////////////////////////////////////
sf::Texture* player;
sf::Texture* playerAtt;
sf::Texture* playerL;
sf::Texture* playerLAtt;

sf::Texture* board;
sf::Texture* devil;
sf::Texture* Dragon;



void client_initialize()
{
	board = new sf::Texture;
	devil = new sf::Texture;
	Dragon = new sf::Texture;
	player = new sf::Texture;
	playerAtt = new sf::Texture;
	playerL = new sf::Texture;
	playerLAtt = new sf::Texture;
	////////////////////// wc //////////////////////////
	ChatUI = new sf::Texture;
	/// ////////////////////////////////////////////////

	board->loadFromFile("Background.jpg");
	player->loadFromFile("Reaper.png");
	devil->loadFromFile("Devil.png");
	Dragon->loadFromFile("Dragon.png");
	playerAtt->loadFromFile("ReaperAtt.png");
	playerL->loadFromFile("Reaper_left.png");
	playerLAtt->loadFromFile("ReaperAtt_left.png");

	////////////////////// wc //////////////////////////
	ChatUI->loadFromFile("chat.png");
	/// ////////////////////////////////////////////////

	if (false == g_font.loadFromFile("cour.ttf")) {
		cout << "Font Loading Error!\n";
		exit(-1);
	}
	white_tile = OBJECT{ *board, 5, 5, TILE_WIDTH, TILE_WIDTH };
	black_tile = OBJECT{ *board, 69, 5, TILE_WIDTH, TILE_WIDTH };
	avatar = OBJECT{ *player,0, 0, 900, 900 };
	avatar.set_scale(0.1, 0.1);
	avatar.move(4, 4);

	////////////////////// wc //////////////////////////
	chatUI = OBJECT{ *ChatUI, 0, 0, 400, 206 };
	/// ////////////////////////////////////////////////
}




void client_finish()
{
	players.clear();
	delete board;
	delete player;

	////////////////////// wc //////////////////////////
	delete ChatUI;
	/// ////////////////////////////////////////////////
}

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN:
	{
		SC_LOGIN_PACKET* packet = reinterpret_cast<SC_LOGIN_PACKET*>(ptr);
		g_myid = packet->id;
		avatar.id = g_myid;
		avatar.move(packet->x, packet->y);
		g_left_x = packet->x - SCREEN_WIDTH / 2;
		g_top_y = packet->y - SCREEN_HEIGHT / 2;
		avatar.show();
	}
	break;

	case SC_ADD_OBJECT:
	{
		SC_ADD_PACKET* my_packet = reinterpret_cast<SC_ADD_PACKET*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
			avatar.show();
		}
		else if (id < MAX_USER) {
			////////////////////// wc //////////////////////////
			//players[id] = OBJECT{ *pieces, 64, 0, 64, 64 };
			players[id] = OBJECT{ *player, 0, 0, 900, 900 };
			players[id].set_scale(0.1, 0.1);
			///////////////////////////////////////////////////

			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].show();
		}
		break;
	}
	case SC_MOVE_PLAYER:
	{
		SC_MOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
		}

		else {
			if (my_packet->left && !players[other_id].m_isleft) {
				players[other_id].m_isleft = true;
				auto previousPosition = players[other_id].m_sprite.getPosition();
				auto currentScale = players[other_id].m_sprite.getScale();
				players[other_id].m_sprite.setTexture(*playerL);
				players[other_id].m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
				players[other_id].m_sprite.setPosition(previousPosition);
				players[other_id].m_sprite.setScale(currentScale);
			}
			else if (my_packet->left == false && players[other_id].m_isleft)
			{

				players[other_id].m_isleft = false;
				auto previousPosition = players[other_id].m_sprite.getPosition();
				auto currentScale = players[other_id].m_sprite.getScale();
				players[other_id].m_sprite.setTexture(*player);
				players[other_id].m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
				players[other_id].m_sprite.setPosition(previousPosition);
				players[other_id].m_sprite.setScale(currentScale);

			}
			players[other_id].move(my_packet->x, my_packet->y);
		}
		break;
	}
	case SC_PLAYER_ATTACK: {
		SC_PLAYER_ATTACK_PACKET* p = reinterpret_cast<SC_PLAYER_ATTACK_PACKET*>(ptr);
		int other_id = p->id;
		if (p->onoff) {
			if (p->id != g_myid && players[other_id].m_isattack == false) {
				players[other_id].m_isattack = p->onoff;
				if (players[other_id].m_isleft) {
					auto previousPosition = players[other_id].m_sprite.getPosition();
					auto currentScale = players[other_id].m_sprite.getScale();
					players[other_id].m_sprite.setTexture(*playerLAtt);
					players[other_id].m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					players[other_id].m_sprite.setPosition(previousPosition);
					players[other_id].m_sprite.setScale(currentScale);
				}
				else
				{
					auto previousPosition = players[other_id].m_sprite.getPosition();
					auto currentScale = players[other_id].m_sprite.getScale();
					players[other_id].m_sprite.setTexture(*playerAtt);
					players[other_id].m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					players[other_id].m_sprite.setPosition(previousPosition);
					players[other_id].m_sprite.setScale(currentScale);
				}
			}
		}
		else
		{
			if (p->id != g_myid && players[other_id].m_isattack == true) {
				players[other_id].m_isattack = p->onoff;
				if (players[other_id].m_isleft) {
					auto previousPosition = players[other_id].m_sprite.getPosition();
					auto currentScale = players[other_id].m_sprite.getScale();
					players[other_id].m_sprite.setTexture(*playerL);
					players[other_id].m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					players[other_id].m_sprite.setPosition(previousPosition);
					players[other_id].m_sprite.setScale(currentScale);
				}
				else
				{
					auto previousPosition = players[other_id].m_sprite.getPosition();
					auto currentScale = players[other_id].m_sprite.getScale();
					players[other_id].m_sprite.setTexture(*player);
					players[other_id].m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					players[other_id].m_sprite.setPosition(previousPosition);

				}
			}
		}
	}
						 break;
	case SC_REMOVE:
	{
		SC_REMOVE_PACKET* my_packet = reinterpret_cast<SC_REMOVE_PACKET*>(ptr);
		
		int other_id = my_packet->id;
		if (my_packet->sessiontype == 1) {

		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			players.erase(other_id);
		}
		}
		else {
			monsters[other_id].hide();
			monsters.erase(other_id);
		}
		break;
	}
	////////////////////// wc //////////////////////////
	/*case SC_CHAT:
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.set_chat(my_packet->mess);
		}
		else {
			players[other_id].set_chat(my_packet->mess);
		}

		break;
	}*/
	/*else if(MAX_USER < id && id < (MAX_NPC / 2)) {
			players[id] = OBJECT{ *devil, 0, 0, 161, 133 };
			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].show();
		}
		else {
			players[id] = OBJECT{ *Dragon, 0, 0, 256, 256 };
			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].show();
		}*/
	case SC_MONTSER_INIT:
	{
		SC_MONTSER_INIT_PACKET* p = reinterpret_cast<SC_MONTSER_INIT_PACKET*>(ptr);
		int npc_id = p->id;
		monsters[npc_id] = OBJECT{ *devil,0,0,161,133 };
		monsters[npc_id].m_sprite.setScale(0.5, 0.5);
		monsters[npc_id].id = p->id;
		monsters[npc_id].move(p->x, p->y);
		monsters[npc_id].set_name("Devil");
		monsters[npc_id].show();
	}
	break;
	case SC_MONSTER_MOVE:
	{
		SC_MONSTER_MOVE_PACKET* p = reinterpret_cast<SC_MONSTER_MOVE_PACKET*>(ptr);
		int npc_id = p->id;
		monsters[npc_id].m_x = p->x;
		monsters[npc_id].m_y = p->y;

	}
	break;
	case SC_MONSTER_REMOVE:
	{
		SC_MONSTER_REMOVE_PACKET* p = reinterpret_cast<SC_MONSTER_REMOVE_PACKET*>(ptr);
		int npc_id = p->id;
		monsters.erase(npc_id);

	}
	break;
	case SC_CHAT:
	{
		SC_CHAT_PACKET* p = reinterpret_cast<SC_CHAT_PACKET*>(ptr);

		string info = "[";
		if (avatar.id == p->id)
			info += to_string(avatar.id);
		else
			info += to_string(p->id);
		info += "]:";
		info += p->message;
		CreateChatMessage(info);

		break;
	}
	///////////////////////////////////////////////////
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = s_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv 에러!";
		exit(-1);
	}
	if (recv_result == sf::Socket::Disconnected) {
		wcout << L"Disconnected\n";
		exit(-1);
	}
	if (recv_result != sf::Socket::NotReady)
		if (received > 0) process_data(net_buf, received);

	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;
			if (0 == (tile_x / 3 + tile_y / 3) % 2) {
				white_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				white_tile.a_draw();
			}
			else
			{
				black_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				black_tile.a_draw();
			}
		}
	avatar.draw();
	for (auto& pl : players) pl.second.draw();
	for (auto& monster : monsters) {
		monster.second.draw();
	}
	sf::Text text;
	text.setFont(g_font);
	char buf[100];
	sprintf_s(buf, "(%d, %d)", avatar.m_x, avatar.m_y);
	text.setString(buf);

	g_window->draw(text);
	////////////////////// wc //////////////////////////
	chatUI.a_move(630, 730);
	chatUI.a_draw();

	chatmessage.setPosition(700, 900);

	int chatSize = curChatMessage.size();

	for (int i = 0; i < chatSize; ++i) {
		curChatMessage[i].setPosition(650, 850 - i * 30);

		g_window->draw(curChatMessage[i]);
	}
	g_window->draw(chatmessage);
	/// ////////////////////////////////////////////////


}

void send_packet(void* packet)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(packet);
	size_t sent = 0;
	s_socket.send(packet, p[0], sent);
}
void Login()
{
	cout << "Name : ";
	cin >> Name;
	CS_LOGIN_PACKET p;
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;

	avatar.set_name(p.name);

	strcpy_s(p.name, Name);
	send_packet(&p);
}
void Attack() {
	CS_ATTACK_PACKET p;
	p.size = sizeof(CS_ATTACK_PACKET);
	p.type = CS_ATTACK;
	send_packet(&p);
}
void Skill(int type) {

}
int main()
{
	wcout.imbue(locale("korean"));
	sf::Socket::Status status = s_socket.connect("127.0.0.1", PORT_NUM);
	s_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"서버와 연결할 수 없습니다.\n";
		exit(-1);
	}

	client_initialize();
	Login();

	sf::Vector2i pos;
	string cchat;
	string info;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "FINAL PROJECT");
	g_window = &window;

	sf::Vector2f previousPosition;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				int direction = -1;
				switch (event.key.code) {
				case sf::Keyboard::Left: {

					direction = 2;
					previousPosition = avatar.m_sprite.getPosition();
					auto currentScale = avatar.m_sprite.getScale();

					// 텍스처만 공격 모션 텍스처로 변경
					avatar.m_sprite.setTexture(*playerL);
					avatar.m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					avatar.mleft = true;
					// 기존 위치와 크기 복원
					avatar.m_sprite.setPosition(previousPosition);
					avatar.m_sprite.setScale(currentScale);
				}
									   break;
				case sf::Keyboard::Right: {

					direction = 3;
					previousPosition = avatar.m_sprite.getPosition();
					auto currentScale = avatar.m_sprite.getScale();

					// 텍스처만 공격 모션 텍스처로 변경
					avatar.m_sprite.setTexture(*player);
					avatar.m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					avatar.mleft = false;
					// 기존 위치와 크기 복원
					avatar.m_sprite.setPosition(previousPosition);
					avatar.m_sprite.setScale(currentScale);
				}
										break;
				case sf::Keyboard::Up:
					direction = 0;
					break;
				case sf::Keyboard::Down:
					direction = 1;
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::Enter:
					CS_CHAT_PACKET chat_packet;
					chat_packet.size = sizeof(chat_packet) - sizeof(chat_packet.message) + strlen(info.c_str()) + 1;
					chat_packet.type = CS_CHAT;

					strcpy_s(chat_packet.message, info.c_str());
					send_packet(&chat_packet);
					info = "";
					g_isChat = false;
					chatmessage.setString(info);
					g_window->draw(chatmessage);
					break;
				case sf::Keyboard::Space:
				{
					previousPosition = avatar.m_sprite.getPosition();
					auto currentScale = avatar.m_sprite.getScale();

					if (avatar.mleft) {
						avatar.m_sprite.setTexture(*playerLAtt);
						avatar.m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					}
					else {
						// 텍스처만 공격 모션 텍스처로 변경
						avatar.m_sprite.setTexture(*playerAtt);
						avatar.m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					}
					// 기존 위치와 크기 복원
					avatar.m_sprite.setPosition(previousPosition);
					avatar.m_sprite.setScale(currentScale);

					Attack();

					break;
				}
				case sf::Keyboard::Q:
				{
					// Skill 1 
				}
				case sf::Keyboard::W:
				{
					// Skill 2 
				}

				default:
					if (g_isChat) {
						info += char(event.key.code) + 97;

						chatmessage.setFont(g_font);
						chatmessage.setString(info);
						chatmessage.setFillColor(sf::Color(255, 255, 255));
					}
					///////////////////////////////////////////////////
					break;
				}
				if (-1 != direction) {
					CS_MOVE_PLAYER_PACKET p;
					p.size = sizeof(p);
					p.type = CS_MOVE_PLAYER;
					p.dir = direction;
					send_packet(&p);
				}

			}
			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::Space) {
					// 스페이스바가 올라왔을 때 원래 모션으로 복귀
					previousPosition = avatar.m_sprite.getPosition();
					auto currentScale = avatar.m_sprite.getScale();
					if (avatar.mleft) {
						avatar.m_sprite.setTexture(*playerL);
						avatar.m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					}
					else {
						avatar.m_sprite.setTexture(*player);
						avatar.m_sprite.setTextureRect(sf::IntRect(0, 0, 900, 900));
					}

					avatar.m_sprite.setPosition(previousPosition);
					avatar.m_sprite.setScale(currentScale);
				}
			}
			////////////////////// wc //////////////////////////		
			if (event.type == sf::Event::MouseButtonPressed)
			{
				switch (event.key.code)
				{
				case sf::Mouse::Left:
					pos = sf::Mouse::getPosition(window);
					if (pos.x > 630 && pos.x < 930 && pos.y > 900 && pos.y < 930)
						g_isChat = !g_isChat;
					break;
				default:
					break;
				}
			}
			///////////////////////////////////////////////////
		}

		window.clear();
		client_main();
		window.display();
	}
	client_finish();

	return 0;
}

////////////////////// wc //////////////////////////
void CreateChatMessage(string _message)
{
	int chatSize = curChatMessage.size();

	if (curChatMessage.size() < 5)
		curChatMessage.push_back(sf::Text());

	if (curChatMessage.size() == 1) {
		SetCurMessage(_message);
	}
	else if (curChatMessage.size() == 2) {
		curChatMessage[1] = curChatMessage[0];
		SetCurMessage(_message);
	}
	else if (curChatMessage.size() == 3) {
		curChatMessage[2] = curChatMessage[1];
		curChatMessage[1] = curChatMessage[0];
		SetCurMessage(_message);
	}
	else if (curChatMessage.size() == 4) {
		curChatMessage[3] = curChatMessage[2];
		curChatMessage[2] = curChatMessage[1];
		curChatMessage[1] = curChatMessage[0];
		SetCurMessage(_message);
	}
	else if (curChatMessage.size() == 5) {
		curChatMessage[4] = curChatMessage[3];
		curChatMessage[3] = curChatMessage[2];
		curChatMessage[2] = curChatMessage[1];
		curChatMessage[1] = curChatMessage[0];
		SetCurMessage(_message);
	}
}

void SetCurMessage(string _message)
{
	curChatMessage[0].setFont(g_font);
	curChatMessage[0].setString(_message);
	curChatMessage[0].setFillColor(sf::Color(255, 255, 255));
	curChatMessage[0].setStyle(sf::Text::Bold);
}

///////////////////////////////////////////////////
