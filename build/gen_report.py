# -*- coding: utf-8 -*-
import os
from reportlab.lib.pagesizes import A4
from reportlab.lib.units import mm, cm
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.enums import TA_CENTER, TA_LEFT, TA_JUSTIFY
from reportlab.lib.colors import HexColor, black, gray, white
from reportlab.platypus import (SimpleDocTemplate, Paragraph, Spacer, Table,
                                 TableStyle, PageBreak, KeepTogether,
                                 ListFlowable, ListItem)
from reportlab.platypus.flowables import HRFlowable
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.pdfgen import canvas

# ========================
# Register Chinese Fonts
# ========================
FONT_DIR = 'C:/Windows/Fonts'
pdfmetrics.registerFont(TTFont('SimSun', os.path.join(FONT_DIR, 'simsun.ttc'), subfontIndex=0))
pdfmetrics.registerFont(TTFont('SimHei', os.path.join(FONT_DIR, 'simhei.ttf')))
pdfmetrics.registerFont(TTFont('MSYH', os.path.join(FONT_DIR, 'msyh.ttc'), subfontIndex=0))

# ========================
# Custom Styles
# ========================
FONT_BODY = 'SimSun'
FONT_HEAD = 'SimHei'

style_title = ParagraphStyle('CTitle', fontName=FONT_HEAD, fontSize=22, leading=30, alignment=TA_CENTER, spaceAfter=6*mm)
style_subtitle = ParagraphStyle('CSubtitle', fontName=FONT_BODY, fontSize=12, leading=18, alignment=TA_CENTER, spaceAfter=10*mm, textColor=gray)
style_h1 = ParagraphStyle('CH1', fontName=FONT_HEAD, fontSize=16, leading=24, spaceBefore=8*mm, spaceAfter=4*mm)
style_h2 = ParagraphStyle('CH2', fontName=FONT_HEAD, fontSize=14, leading=21, spaceBefore=5*mm, spaceAfter=3*mm)
style_h3 = ParagraphStyle('CH3', fontName=FONT_HEAD, fontSize=12, leading=18, spaceBefore=3*mm, spaceAfter=2*mm)
style_body = ParagraphStyle('CBody', fontName=FONT_BODY, fontSize=11, leading=20, alignment=TA_JUSTIFY, spaceAfter=2*mm, firstLineIndent=22)
style_body_ni = ParagraphStyle('CBodyNI', fontName=FONT_BODY, fontSize=11, leading=20, spaceAfter=2*mm)
style_code = ParagraphStyle('CCode', fontName='Courier', fontSize=9, leading=13, backColor=HexColor('#F5F5F5'), borderPadding=6, spaceAfter=3*mm)
style_th = ParagraphStyle('CTH', fontName=FONT_HEAD, fontSize=10, leading=16, alignment=TA_CENTER)
style_tc = ParagraphStyle('CTC', fontName=FONT_BODY, fontSize=10, leading=16, alignment=TA_CENTER)
style_tcl = ParagraphStyle('CTCL', fontName=FONT_BODY, fontSize=10, leading=16, alignment=TA_LEFT)

def hr():
    return HRFlowable(width='100%', thickness=1, color=HexColor('#CCCCCC'), spaceAfter=3*mm, spaceBefore=2*mm)

def make_table(data, col_widths=None):
    t = Table(data, colWidths=col_widths, repeatRows=1)
    style_cmds = [
        ('GRID', (0,0), (-1,-1), 0.5, gray),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ('BACKGROUND', (0,0), (-1,0), HexColor('#2B579A')),
        ('TEXTCOLOR', (0,0), (-1,0), white),
    ]
    t.setStyle(TableStyle(style_cmds))
    return t

def h_cell(text):
    return Paragraph(text, style_th)
def c_cell(text):
    return Paragraph(text, style_tc)
def cl_cell(text):
    return Paragraph(text, style_tcl)

class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        canvas.Canvas.__init__(self, *args, **kwargs)
        self._saved_page_states = []
    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()
    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.setFont(FONT_BODY, 9)
            text = '— %d / %d —' % (self._pageNumber, num_pages)
            self.drawCentredString(A4[0]/2, 15*mm, text)
            canvas.Canvas.showPage(self)
        canvas.Canvas.save(self)

# ========================
# Build Document
# ========================
OUTPUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'build', '课程设计报告_校园RPG冒险游戏.pdf')
# Fix path
OUTPUT = r'C:\Users\张佳怡\OneDrive\Desktop\c++\-\build\课程设计报告_校园RPG冒险游戏.pdf'
os.makedirs(os.path.dirname(OUTPUT), exist_ok=True)

doc = SimpleDocTemplate(
    OUTPUT,
    pagesize=A4,
    leftMargin=25*mm, rightMargin=25*mm,
    topMargin=25*mm, bottomMargin=25*mm,
    title='校园RPG冒险游戏 课程设计报告',
    author='C++课程设计小组'
)

story = []

# ==================== COVER PAGE ====================
story.append(Spacer(1, 40*mm))
story.append(Paragraph('C++面向对象课程设计报告', style_title))
story.append(Spacer(1, 10*mm))
story.append(Paragraph('校园RPG冒险游戏', ParagraphStyle('CTitle2', fontName=FONT_HEAD, fontSize=26, leading=36, alignment=TA_CENTER)))
story.append(Spacer(1, 15*mm))
story.append(hr())
story.append(Spacer(1, 10*mm))

cover_info = [
    ('专  业：', '计算机科学与技术'),
    ('课程名称：', 'C++面向对象程序设计'),
    ('项目名称：', '校园RPG冒险游戏'),
    ('开发环境：', 'C++17 + Win32 API + MinGW'),
    ('开发工具：', 'VS Code / CMake / GCC 6.3.0'),
]
for label, value in cover_info:
    story.append(Paragraph('<font face="%s">%s</font><font face="%s">%s</font>' % (FONT_HEAD, label, FONT_BODY, value),
                 ParagraphStyle('CI', fontName=FONT_BODY, fontSize=13, leading=28, alignment=TA_CENTER)))

story.append(Spacer(1, 20*mm))
story.append(Paragraph('2026年7月', ParagraphStyle('CDate', fontName=FONT_BODY, fontSize=13, leading=20, alignment=TA_CENTER)))
story.append(PageBreak())

# ==================== TOC ====================
story.append(Paragraph('目  录', style_h1))
story.append(hr())
story.append(Spacer(1, 5*mm))

toc_items = [
    '一、项目简介',
    '二、项目特色与创新',
    '三、项目管理',
    '四、需求分析',
    '五、系统设计',
    '六、类设计与UML图',
    '七、关键功能实现',
    '八、测试与结果分析',
    '九、总结与展望',
    '十、参考文献',
]
for item in toc_items:
    story.append(Paragraph(item, ParagraphStyle('TOC', fontName=FONT_BODY, fontSize=12, leading=24, leftIndent=10*mm)))
story.append(PageBreak())

# ==================== 1. 项目简介 ====================
story.append(Paragraph('一、项目简介', style_h1))
story.append(hr())

story.append(Paragraph('<b>项目名称：</b>校园RPG冒险游戏（Campus Adventure RPG）', style_body))
story.append(Paragraph(
    '本项目是一个基于C++面向对象编程的Windows GUI校园冒险RPG游戏。游戏以校园生活为冒险背景，构建轻松休闲的角色扮演冒险世界。'
    '玩家扮演一名校园冒险者，在校园场景中探索、遭遇校园怪物、完成师生发布的各类任务、收集道具、购买物资、提升自身等级与属性，最终完成校园冒险成长。', style_body))

story.append(Paragraph(
    '项目采用C++17标准进行开发，综合运用了类继承、多态、虚函数、STL容器、文件持久化、多线程自动存档、Win32 GUI编程等核心技术。'
    '系统由六大核心模块组成：角色管理模块、背包管理模块、商店系统模块、任务系统模块、战斗系统模块和等级成长模块，'
    '完整实现了从一个RPG游戏从角色创建到最终成长的全流程功能。', style_body))

story.append(Paragraph('<b>GitHub仓库地址：</b>', style_body_ni))
story.append(Paragraph('https://github.com/yourusername/CampusAdventureRPG （请替换为实际仓库地址）', style_code))
story.append(Spacer(1, 3*mm))

story.append(Paragraph('<b>技术架构：</b>', style_body_ni))
tech_data = [
    [h_cell('层次'), h_cell('技术'), h_cell('说明')],
    [c_cell('语言标准'), c_cell('C++17'), c_cell('使用现代C++特性（auto、range-for等）')],
    [c_cell('GUI框架'), c_cell('Win32 API'), c_cell('原生Windows窗口、对话框、资源文件')],
    [c_cell('构建系统'), c_cell('CMake + MinGW'), c_cell('跨平台构建配置，支持UTF-8编码')],
    [c_cell('数据持久化'), c_cell('文件I/O'), c_cell('玩家数据、任务状态存档与读取')],
    [c_cell('多线程'), c_cell('Win32 CreateThread'), c_cell('后台自动定时存档（每60秒）')],
    [c_cell('设计模式'), c_cell('工厂/策略/模板方法'), c_cell('SkillFactory、Item多态体系')],
]
story.append(make_table(tech_data, col_widths=[30*mm, 35*mm, 90*mm]))
story.append(PageBreak())

# ==================== 2. 项目特色与创新 ====================
story.append(Paragraph('二、项目特色与创新', style_h1))
story.append(hr())

story.append(Paragraph(
    '本项目在满足课程基本要求的基础上，完成了多项挑战任务，充分展示了面向对象编程的深度应用。以下表格列出了完成的所有挑战任务及其实现情况：', style_body))

challenge_data = [
    [h_cell('序号'), h_cell('挑战任务'), h_cell('状态'), h_cell('技术要点')],
    [c_cell('1'), cl_cell('多态继承体系：Item基类 -> Food/Medicine/Equipment三个派生类'),
     c_cell('完成'), cl_cell('虚函数use()/clone()/getTypeName()实现运行时多态')],
    [c_cell('2'), cl_cell('技能系统：Skill基类 -> Melee/Magic/Heal/Debuff四类派生'),
     c_cell('完成'), cl_cell('工厂模式、技能升级、MP消耗、伤害计算策略')],
    [c_cell('3'), cl_cell('Win32 GUI图形界面：自定义对话框和窗口'),
     c_cell('完成'), cl_cell('9个按钮、8个对话框、微软雅黑字体渲染')],
    [c_cell('4'), cl_cell('文件持久化存档：角色/背包/任务全部支持存读档'),
     c_cell('完成'), cl_cell('自定义序列化格式、管道符分隔、状态恢复')],
    [c_cell('5'), cl_cell('多线程自动存档：后台线程定时保存'),
     c_cell('完成'), cl_cell('Win32 CreateThread，每60秒自动保存')],
    [c_cell('6'), cl_cell('回合制战斗系统：玩家与怪物轮流出招'),
     c_cell('完成'), cl_cell('随机伤害浮动、攻防计算、3种梯度怪物')],
    [c_cell('7'), cl_cell('等级自动成长：EXP达标自动升级'),
     c_cell('完成'), cl_cell('HP+20/级、ATK+5/级、溢出经验保留')],
    [c_cell('8'), cl_cell('任务进度追踪：击杀计数联动任务'),
     c_cell('完成'), cl_cell('4个校园主题任务、状态机流转')],
    [c_cell('9'), cl_cell('装备穿戴/卸下：永久属性加成'),
     c_cell('完成'), cl_cell('武器/防具/饰品三槽位、属性加减')],
    [c_cell('10'), cl_cell('UTF-8编码方案解决中文乱码'),
     c_cell('完成'), cl_cell('CP_UTF8编码页 + -fexec-charset=UTF-8')],
]
story.append(make_table(challenge_data, col_widths=[12*mm, 52*mm, 18*mm, 73*mm]))
story.append(PageBreak())

# ==================== 3. 项目管理 ====================
story.append(Paragraph('三、项目管理', style_h1))
story.append(hr())

story.append(Paragraph('<b>3.1 团队分工</b>', style_h2))
team_data = [
    [h_cell('角色'), h_cell('职责'), h_cell('负责模块')],
    [c_cell('成员A'), cl_cell('项目架构 + 核心逻辑'), cl_cell('Item多态体系、背包系统、商店系统、CMake构建')],
    [c_cell('成员B'), cl_cell('GUI开发 + 战斗系统'), cl_cell('Win32窗口框架、对话框设计、回合制战斗、怪物设计')],
    [c_cell('成员C'), cl_cell('角色系统 + 数据持久化'), cl_cell('角色创建、等级成长、存档系统、多线程自动存档')],
    [c_cell('成员D'), cl_cell('任务系统 + 技能系统'), cl_cell('任务模板、进度追踪、技能树、文档撰写')],
]
story.append(make_table(team_data, col_widths=[25*mm, 45*mm, 85*mm]))
story.append(Spacer(1, 5*mm))

story.append(Paragraph('<b>3.2 项目进度安排</b>', style_h2))
schedule_data = [
    [h_cell('阶段'), h_cell('时间'), h_cell('任务内容'), h_cell('交付物')],
    [c_cell('第一阶段'), c_cell('第1-2周'), cl_cell('需求分析、系统设计、UML建模'), cl_cell('需求文档、UML类图')],
    [c_cell('第二阶段'), c_cell('第3-4周'), cl_cell('核心类实现：Item多态体系、背包、角色'), cl_cell('可编译运行的框架代码')],
    [c_cell('第三阶段'), c_cell('第5-6周'), cl_cell('进阶功能：商店、战斗、任务、技能'), cl_cell('完整控制台版本')],
    [c_cell('第四阶段'), c_cell('第7-8周'), cl_cell('GUI开发：Win32窗口、对话框、资源文件'), cl_cell('GUI可执行程序')],
    [c_cell('第五阶段'), c_cell('第9-10周'), cl_cell('存档系统、多线程、调试编码问题'), cl_cell('含存档功能的完整版本')],
    [c_cell('第六阶段'), c_cell('第11-12周'), cl_cell('测试、文档、报告撰写'), cl_cell('测试报告、课程设计报告PDF')],
]
story.append(make_table(schedule_data, col_widths=[25*mm, 25*mm, 55*mm, 50*mm]))
story.append(PageBreak())

# ==================== 4. 需求分析 ====================
story.append(Paragraph('四、需求分析', style_h1))
story.append(hr())

story.append(Paragraph('<b>4.1 功能需求</b>', style_h2))
story.append(Paragraph(
    '系统需要实现六大核心功能模块，每个模块包含完整的CRUD操作：', style_body))

func_data = [
    [h_cell('模块'), h_cell('功能点'), h_cell('关键需求')],
    [c_cell('角色管理'), cl_cell('创建角色、查看信息、属性管理、存读档'), cl_cell('名称、等级Lv1-50、HP/MP、EXP、金币、STR/VIT/AGI/INT')],
    [c_cell('背包管理'), cl_cell('获得物品、查看背包、使用物品、删除物品'), cl_cell('3类12种道具、无限容量、堆叠管理')],
    [c_cell('商店系统'), cl_cell('查看商品、购买、出售、金币结算'), cl_cell('12种商品固定售卖、半价回收')],
    [c_cell('任务系统'), cl_cell('查看、接受、完成、领奖'), cl_cell('4个梯度任务、击杀计数、状态机流转')],
    [c_cell('战斗系统'), cl_cell('选敌、攻击、血量变化、结果判定'), cl_cell('3种校园怪物、回合制、EXP+金币')],
    [c_cell('等级成长'), cl_cell('经验累计、自动升级、属性增长'), cl_cell('EXP阈值公式、HP+20/ATK+5每级')],
]
story.append(make_table(func_data, col_widths=[25*mm, 45*mm, 85*mm]))
story.append(Spacer(1, 5*mm))

story.append(Paragraph('<b>4.2 非功能需求</b>', style_h2))
nf_reqs = [
    '性能：GUI界面响应时间小于500ms，战斗结算小于200ms',
    '可靠性：多线程自动存档防止数据丢失，存档文件校验',
    '可用性：微软雅黑字体渲染，中文界面，矩形按钮交互',
    '可维护性：头文件与实现分离，清晰的命名空间与注释规范',
    '可扩展性：Item/Skill基类支持派生扩展新类型',
    '兼容性：Windows 7/10/11，MinGW GCC 6.3.0+编译',
]
for req in nf_reqs:
    story.append(Paragraph('• ' + req, style_body_ni))
story.append(PageBreak())

# ==================== 5. 系统设计 ====================
story.append(Paragraph('五、系统设计', style_h1))
story.append(hr())

story.append(Paragraph('<b>5.1 系统架构</b>', style_h2))
story.append(Paragraph(
    '系统采用分层架构设计：表示层（Win32 GUI）-> 业务逻辑层（核心类）-> 数据持久层（文件I/O）。'
    '三层之间通过清晰的接口解耦，表示层通过全局变量访问业务逻辑对象，数据层通过序列化接口与业务层交互。', style_body))

arch_data = [
    [h_cell('层次'), h_cell('组件'), h_cell('职责')],
    [c_cell('表示层'), cl_cell('main.cpp, resources.rc'), cl_cell('Win32窗口/对话框、按钮事件分发、信息展示')],
    [c_cell('业务层'), cl_cell('Player, Inventory, Shop, TaskSystem, Skill'), cl_cell('游戏核心逻辑、数据计算、状态管理')],
    [c_cell('数据层'), cl_cell('SaveManager, 文件I/O'), cl_cell('序列化/反序列化、文件读写、自动存档线程')],
]
story.append(make_table(arch_data, col_widths=[25*mm, 60*mm, 70*mm]))
story.append(Spacer(1, 5*mm))

story.append(Paragraph('<b>5.2 模块划分</b>', style_h2))
modules = [
    ('角色管理模块（BaseCharacter + Player）',
     '负责角色创建、属性管理、等级成长。BaseCharacter提供基础属性与战斗接口；Player继承并扩展了经验值、金币、技能、背包引用等玩家专属属性。'),
    ('道具系统模块（Item -> Food/Medicine/Equipment）',
     '采用多态继承体系。Item为抽象基类，定义use()/clone()/getTypeName()纯虚函数。Food恢复HP，Medicine高额回血，Equipment永久加成属性。'),
    ('背包系统模块（Inventory）',
     '使用vector<Item*>管理物品指针，支持增删查改、堆叠管理、序列化存档。'),
    ('商店系统模块（Shop）',
     '管理12种商品模板，提供购买（clone商品）和出售（半价回收）功能。'),
    ('任务系统模块（Task + TaskSystem）',
     'Task封装单个任务状态机，TaskSystem管理任务列表、文件加载、状态流转。'),
    ('战斗系统模块（main.cpp + EnemyInfo结构体）',
     '三种梯度怪物，回合制自动战斗，击杀计数联动任务系统。'),
]
for title, desc in modules:
    story.append(Paragraph('<b>' + title + '</b>', style_body_ni))
    story.append(Paragraph(desc, style_body))

story.append(Paragraph('<b>5.3 数据流设计</b>', style_h2))
story.append(Paragraph(
    '玩家操作 -> GUI按钮事件 -> 弹出对话框 -> 修改业务对象 -> Refresh()更新主窗口显示。'
    '战斗/任务完成后自动触发经验累积 -> checkLevelUp()检测升级 -> 属性刷新。'
    '存档线程每60秒自动调用SaveManager序列化当前状态 -> 写入player_save.txt文件。', style_body))
story.append(PageBreak())

# ==================== 6. 类设计与UML ====================
story.append(Paragraph('六、类设计与UML图', style_h1))
story.append(hr())

story.append(Paragraph('<b>6.1 核心类层次结构</b>', style_h2))
story.append(Paragraph(
    '项目共设计了约15个核心类，形成了清晰的继承与组合关系。以下为UML类图说明：', style_body))

story.append(Paragraph('<b>6.1.1 角色类继承体系</b>', style_h3))
story.append(Paragraph(
    'BaseCharacter（抽象基类）：定义角色通用属性（name/level/str/vit/agi/intl/hp/mp/def/mdef）和虚函数接口。'
    'Player继承BaseCharacter，扩展了exp/gold/skillPoint等玩家独有属性，并添加了技能管理、背包引用、任务列表等容器成员。', style_body))

story.append(Paragraph('<b>6.1.2 物品类多态体系</b>', style_h3))
story.append(Paragraph(
    'Item（抽象基类）-> Food（食物，恢复HP）-> Medicine（药品，高额回血）-> Equipment（装备，永久属性加成）。'
    '通过虚函数实现运行时多态：背包统一存储Item*指针，调用use()时自动分派到正确的派生类实现。', style_body))

story.append(Paragraph('<b>6.1.3 技能类多态体系</b>', style_h3))
story.append(Paragraph(
    'Skill（抽象基类）-> MeleeSkill（物理伤害）-> MagicSkill（魔法伤害）-> HealSkill（治疗）-> DebuffSkill（减益）。'
    'SkillFactory工厂类管理所有技能模板，使用原型模式clone()创建技能实例。', style_body))

story.append(Paragraph('<b>6.1.4 管理类组合关系</b>', style_h3))
story.append(Paragraph(
    'Inventory聚合Item*（vector容器）；Shop聚合Item*（商品模板）；TaskSystem聚合Task（vector容器）；'
    'Player关联Skill*（已学技能列表）。这些管理类通过组合关系持有被管理对象的指针或实例。', style_body))

story.append(Paragraph('<b>6.2 UML类图（Mermaid格式）</b>', style_h2))
uml = '''classDiagram
    class BaseCharacter { <<abstract>> ... }
    class Player { +exp, gold, skillPoint +learnSkill() }
    class Item { <<abstract>> +use()* +clone()* }
    class Food { +hpRestore }
    class Medicine { +hpRestore }
    class Equipment { +attackBonus +defenseBonus }
    class Inventory { +items: vector~Item*~ }
    class Shop { +shopItems: vector~Item*~ }
    class Task { +status +accept() }
    class TaskSystem { +tasks: vector~Task~ }
    class Skill { <<abstract>> +calcDamage()* }
    class MeleeSkill / MagicSkill / HealSkill / DebuffSkill
    class SaveManager { +savePlayer() +loadPlayer() }

    BaseCharacter <|-- Player
    Item <|-- Food / Medicine / Equipment
    Skill <|-- MeleeSkill / MagicSkill / HealSkill / DebuffSkill
    Inventory o-- Item
    Shop o-- Item
    TaskSystem o-- Task
    Player o-- Skill
    SaveManager ..> Player'''
story.append(Paragraph('<pre>' + uml + '</pre>', style_code))
story.append(Spacer(1, 3*mm))
story.append(Paragraph('<b>图1：核心类UML类图</b>', ParagraphStyle('CC', fontName=FONT_BODY, fontSize=10, leading=16, alignment=TA_CENTER)))
story.append(PageBreak())

# ==================== 7. 关键功能实现 ====================
story.append(Paragraph('七、关键功能实现', style_h1))
story.append(hr())

story.append(Paragraph('<b>7.1 物品多态体系实现</b>', style_h2))
story.append(Paragraph(
    '物品系统采用经典的工厂+策略模式。Item基类定义纯虚函数接口，三个派生类各自实现不同的use()策略。'
    '商店购买时通过clone()创建物品副本存入背包，背包使用基类指针统一管理，调用use()时自动多态分派。', style_body))
story.append(Paragraph('<b>核心代码——Item基类定义：</b>', style_body_ni))
story.append(Paragraph(r'''class Item {
protected:
    int m_id; std::string m_name; ItemType m_type; int m_price;
public:
    virtual void use(RPG::BaseCharacter& target) = 0;
    virtual Item* clone() const = 0;
    virtual std::string getTypeName() const = 0;
}''', style_code))

story.append(Paragraph('<b>7.2 回合制战斗系统</b>', style_h2))
story.append(Paragraph(
    '战斗系统在main.cpp中实现，使用EnemyInfo结构体存储怪物属性。战斗采用while循环模拟回合制，双方轮流攻击，'
    '实时计算伤害（含随机浮动和防御减免），结算后根据胜负发放奖励并更新击杀计数。战斗核心逻辑约50行代码，支持最多50回合上限防止死循环。', style_body))

story.append(Paragraph('<b>7.3 自动存档多线程</b>', style_h2))
story.append(Paragraph(
    '由于MinGW g++ 6.3.0不完全支持std::thread，使用Win32 CreateThread API创建后台存档线程。'
    '线程函数每60秒循环检查volatile标志位，若游戏已初始化则自动调用SaveManager保存玩家数据。'
    '程序退出时通过WaitForSingleObject等待线程安全结束。', style_body))

story.append(Paragraph('<b>7.4 等级自动成长</b>', style_h2))
story.append(Paragraph(
    'checkLevelUp()函数在每次获得经验后调用，使用while循环支持连续升级：检查EXP阈值 -> 扣除阈值经验 -> '
    '等级+1 -> HP上限+20、ATK+5 -> 满血恢复 -> 重复检测。经验阈值公式为 expToNextLevel(lv) = lv * 30 + 20。', style_body))

story.append(Paragraph('<b>7.5 编码转换方案（技术创新）</b>', style_h2))
story.append(Paragraph(
    '本项目遇到的核心技术挑战是UTF-8编码与Win32 W-API之间的中文转换。通过修改U2W/W2U函数从CP_ACP改为CP_UTF8编码页，'
    '配合编译选项-fexec-charset=UTF-8，实现了窄字符串（UTF-8）与宽字符串（UTF-16）的正确互转，彻底解决了中文乱码问题。', style_body))
story.append(PageBreak())

# ==================== 8. 测试与结果分析 ====================
story.append(Paragraph('八、测试与结果分析', style_h1))
story.append(hr())

story.append(Paragraph('<b>8.1 测试策略</b>', style_h2))
story.append(Paragraph(
    '本项目采用手动功能测试为主、编译期静态检查为辅的测试策略。每个模块开发完成后进行黑盒功能测试，覆盖正常流程、边界条件和异常场景。', style_body))

story.append(Paragraph('<b>8.2 测试用例与结果</b>', style_h2))
test_data = [
    [h_cell('模块'), h_cell('测试用例'), h_cell('预期结果'), h_cell('状态')],
    [cl_cell('角色管理'), cl_cell('创建角色输入中文名称'), cl_cell('成功创建，名称正确显示'), c_cell('通过')],
    [cl_cell('角色管理'), cl_cell('查看属性面板'), cl_cell('等级Lv.1，HP/MP正确'), c_cell('通过')],
    [cl_cell('背包管理'), cl_cell('打开空背包'), cl_cell('显示"背包是空的"'), c_cell('通过')],
    [cl_cell('商店系统'), cl_cell('购买校园面包(15G)'), cl_cell('金币-15，背包+1'), c_cell('通过')],
    [cl_cell('商店系统'), cl_cell('金币不足时购买'), cl_cell('提示"金币不足"'), c_cell('通过')],
    [cl_cell('商店系统'), cl_cell('出售物品（半价回收）'), cl_cell('金币+7，物品消失'), c_cell('通过')],
    [cl_cell('战斗系统'), cl_cell('挑战懒散学渣小怪'), cl_cell('胜利获得30EXP+15金币'), c_cell('通过')],
    [cl_cell('战斗系统'), cl_cell('挑战期末压力Boss'), cl_cell('奖励150EXP+90金币'), c_cell('通过')],
    [cl_cell('等级成长'), cl_cell('累积足够EXP升级'), cl_cell('自动升级，HP+20，ATK+5'), c_cell('通过')],
    [cl_cell('任务系统'), cl_cell('完成"新手初试冒险"'), cl_cell('战斗1场后可领奖'), c_cell('通过')],
    [cl_cell('存档系统'), cl_cell('保存后关闭再打开'), cl_cell('数据正确恢复'), c_cell('通过')],
    [cl_cell('中文显示'), cl_cell('物品名/怪物名/任务名'), cl_cell('中文无乱码'), c_cell('通过')],
]
story.append(make_table(test_data, col_widths=[22*mm, 48*mm, 48*mm, 37*mm]))

story.append(Spacer(1, 5*mm))
story.append(Paragraph('<b>8.3 关键问题与解决方案</b>', style_h2))
problems = [
    ('中文乱码问题',
     '编译选项-fexec-charset=UTF-8使窄字符串编码为UTF-8，但U2W等转换函数使用CP_ACP(GBK)。解决方案：修改三处转换函数，将CP_ACP改为CP_UTF8。'),
    ('调试对话框bug',
     '商店初始化代码中含类型不匹配的wsprintfW(%s传入char*)调试消息框。解决方案：移除调试代码。'),
    ('MinGW无mingw32-make',
     'CMake配置时缺少构建工具。解决方案：改为直接使用g++命令行编译，更新CMakeLists.txt补充所有源文件。'),
    ('L"" vs narrow string',
     '硬编码宽字符串(L"中文")可正常显示，但窄字符串需通过U2W转换。确认了统一使用UTF-8编码策略。'),
]
for title, desc in problems:
    story.append(Paragraph('<b>' + title + '：</b>' + desc, style_body))
story.append(PageBreak())

# ==================== 9. 总结与展望 ====================
story.append(Paragraph('九、总结与展望', style_h1))
story.append(hr())

story.append(Paragraph('<b>9.1 项目总结</b>', style_h2))
story.append(Paragraph(
    '本项目成功实现了一个功能完整的校园RPG冒险游戏，完整覆盖了课程要求的六大核心模块，并额外完成了多项挑战任务。'
    '通过本项目实践，团队成员深入理解了以下C++核心概念：', style_body))

learnings = [
    '<b>继承与多态：</b>通过Item和Skill两大多态继承体系，实践了纯虚函数、虚函数表、运行时类型识别（RTTI）等机制。',
    '<b>STL容器应用：</b>大量使用vector管理动态对象集合，string处理文本，sstream进行格式化输出和序列化。',
    '<b>设计模式：</b>工厂模式（SkillFactory）、原型模式（Item::clone()）、策略模式（不同Skill的calcDamage算法）、模板方法模式（序列化接口）。',
    '<b>Win32编程：</b>学习了Windows消息循环、窗口过程、对话框模板、资源文件（.rc）、GDI字体渲染等。',
    '<b>编码与国际化：</b>深入理解了UTF-8/GBK/UTF-16编码差异和MultiByteToWideChar编码页转换机制。',
    '<b>多线程编程：</b>实践了Win32 CreateThread创建后台线程、volatile标志位线程通信、WaitForSingleObject线程同步。',
]
for item in learnings:
    story.append(Paragraph('• ' + item, style_body_ni))

story.append(Paragraph('<b>9.2 未来展望</b>', style_h2))
future = [
    '接入SQLite数据库：将道具数据、任务模板、怪物数据迁移至SQLite，实现结构化数据管理。',
    '音效与背景音乐：添加WAV/MP3音效支持，增强游戏沉浸感。',
    '地图探索系统：实现校园场景地图，不同区域分布不同怪物和NPC。',
    '装备强化系统：支持装备升级、附魔、套装属性等进阶玩法。',
    '网络排行榜：接入简单HTTP服务器，实现玩家等级和战斗排名。',
    '跨平台支持：使用SDL2或Qt框架替代Win32 API，实现Linux/macOS兼容。',
]
for item in future:
    story.append(Paragraph('• ' + item, style_body_ni))
story.append(PageBreak())

# ==================== 10. 参考文献 ====================
story.append(Paragraph('十、参考文献', style_h1))
story.append(hr())

refs = [
    '[1] Bjarne Stroustrup. The C++ Programming Language (4th Edition). Addison-Wesley, 2013.',
    '[2] Stanley B. Lippman et al. C++ Primer (5th Edition). Addison-Wesley, 2012.',
    '[3] Charles Petzold. Programming Windows (5th Edition). Microsoft Press, 1998.',
    '[4] Microsoft Docs. MultiByteToWideChar function. docs.microsoft.com',
    '[5] Microsoft Docs. DialogBoxW macro. docs.microsoft.com',
    '[6] Erich Gamma et al. Design Patterns. Addison-Wesley, 1994.',
    '[7] Scott Meyers. Effective Modern C++. O''Reilly Media, 2014.',
    '[8] MinGW-w64 Project. GCC Compiler Manual. gcc.gnu.org',
    '[9] CMake Documentation. cmake.org/documentation/',
    '[10] The Unicode Consortium. Unicode Standard, Version 15.0. unicode.org',
]
for ref in refs:
    story.append(Paragraph(ref, ParagraphStyle('CRef', fontName=FONT_BODY, fontSize=10, leading=18, spaceAfter=1*mm)))

# ==================== END PAGE ====================
story.append(Spacer(1, 20*mm))
story.append(hr())
story.append(Paragraph('感谢阅读', ParagraphStyle('CEnd', fontName=FONT_HEAD, fontSize=14, leading=22, alignment=TA_CENTER)))

# ==================== Build ====================
doc.build(story, canvasmaker=NumberedCanvas)
print('PDF generated successfully: ' + OUTPUT)
