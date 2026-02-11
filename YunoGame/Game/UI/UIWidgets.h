#pragma once
// 루트 클래스
#include "Widget.h"
//#include "LogicWidget.h"
/*
중간 클래스 구조도
Image,
Button,
Text,
Slot,
ProgressBar,
Gauge, // ProgressBar의 확정 자식
*/


//////////////////////////////
// 디버그용 클래스
#include "WidgetGridLine.h" // 디버그 그리드 생성

//////////////////////////////

//////////////////////////////
// Image 클래스
#include "Image.h"

// Image 파생 클래스
#include "Letterbox.h"
#include "PhasePanel.h"
#include "CardSelectionPanel.h"
#include "CardConfirmPanel.h"
#include "AddCardPanel.h"
#include "UserImage.h"
#include "PlayerIcon.h"
#include "TitleImage.h"
#include "Emoji.h"
#include "Minimap.h"
#include "MinimapTile.h"
#include "TextureImage.h"
#include "ShowCardDeck.h"
//////////////////////////////


//////////////////////////////
// Button 클래스
#include "Button.h"

// Button 파생 클래스
#include "Card.h"
#include "ReadyButton.h"
#include "ExitButton.h"
#include "WeaponButton.h"
#include "CardConfirmButton.h"
#include "CardCancelButton.h"
#include "SceneChangeButton.h"
#include "OptionButton.h"
#include "PopButton.h"
//////////////////////////////


//////////////////////////////
// Text 클래스
#include "Text.h"

// Text 파생 클래스
// #include ".h"
//////////////////////////////


//////////////////////////////
// Slot 클래스
#include "Slot.h"

// Slot 파생 클래스
#include "CardSlot.h"
//////////////////////////////


//////////////////////////////
// ProgressBar 클래스
#include "ProgressBar.h"

// ProgressBar 파생 클래스
#include "HealthBar.h"
#include "StaminaBar.h"


// Gauge 클래스
#include "Gauge.h"

// ProgressBar 파생 클래스
#include "HealthGauge.h"
#include "StaminaGauge.h"
//////////////////////////////






