#pragma once
#include <cstdint>


    enum class CurrentSceneState : std::uint8_t
    {
        Title = 0,      // 타이틀 화면
        EscScene,
        ResultScene,
        Option,
        Guide,
        RequstEnter,    // 입장 요청
        GameStart,      // 무기 선택 화면 (양 쪽 다 무기를 골라야 RountStart로 넘어감)
        StandBy,        // 
        CountDown,
        RoundStart,     // 게임 초기화 단계 (기물 배치, 오브젝트 스폰, )
        SubmitCard,
        BattleStandBy,  // 나는 카드 제출 후 대기중인 상태
        AutoBattle,
        RoundEnd,
        GameEnd,
    };

