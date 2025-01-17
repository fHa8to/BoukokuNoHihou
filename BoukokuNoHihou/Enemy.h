#include "DxLib.h"
#include <math.h>
#include <memory>

// �ő�g�o
#define ENEMY_HP_MAX		10
// �g�o�P�łǂꂾ���o�[��L�΂���
#define ENEMY_DRAW_SIZE	50

//���C���g���[�X�̓_��
#define MAX_POINT 6

class Player;
class Enemy
{
public:
	Enemy();
	virtual ~Enemy();

	void Init();
	void Update(std::shared_ptr<Player> m_pPlayer);
	void Draw();
	void End();

	//�G�l�~�[�̍��W���擾
	VECTOR& GetPos() { return m_pos; }
	void SetPos(const VECTOR pos) { m_pos = pos; }

	//�G��HP���擾
	const int& GetHp() const { return m_hp; }
	void SetHp(const int hp) { m_hp = hp; }

	float GetRadius() { return m_modelRadius; }

	//�U���̎擾
	const bool& GetUnderAttack() const { return m_isUnderAttack; }

	int SetColor(int color) { return m_color = color; }

	enum State
	{
		kIdle,		//�ҋ@
		kRun,		//����
		kAttack,	//�U��
		kDeath,		//��
	};

	State GetState() { return m_state; }
	void SetState(State state) { m_state = state; }

	bool Translation(std::shared_ptr<Player> m_pPlayer);

	//�U���̓����蔻��
	bool IsAttackColliding(std::shared_ptr<Player> m_pPlayer);

private:
	//�A�j���[�V�����̐i�s
//���[�v�������ǂ�����Ԃ�
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);


private:
	//���f���n���h��
	int m_modelHandle;

	//�A�j���[�V�������
	int m_animIndex;
	int m_currentAnimNo;	//���݂̃A�j���[�V����
	int m_prevAnimNo;		//�ύX�O�̃A�j���[�V����
	float m_animBlendRate;	//�A�j���[�V������������


	//�\�����
	VECTOR m_pos;
	VECTOR m_attackPos;
	VECTOR m_angle;
	VECTOR m_playerPos;
	VECTOR m_direction;


	//�i�ދ���
	VECTOR m_distance;

	VECTOR PointPos[MAX_POINT];
	VECTOR PointPos1[MAX_POINT];

	//�����蔻��̔��a
	float m_modelRadius;
	float m_discoveryRadius;
	float m_attackRadius;

	int TargetNumber;


	//�A�j���[�V�����t���O
	int m_isIdle;
	int m_isAttack;
	int m_isRnu;
	int m_isDead;


	//State�ϐ�
	State m_state;

	//�����蔻��̔����t���O
	bool m_isUnderAttack;

	int m_color = 0xffffff;

	//HP
	int m_hp;

};