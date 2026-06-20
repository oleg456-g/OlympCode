from datetime import datetime
from sqlalchemy import Integer, String, Text, Float, DateTime, ForeignKey, Boolean, Enum as SAEnum
from sqlalchemy.orm import Mapped, mapped_column, relationship
from database import Base
import enum


class Verdict(str, enum.Enum):
    PENDING = "PENDING"
    RUNNING = "RUNNING"
    AC      = "AC"
    WA      = "WA"
    TLE     = "TLE"
    MLE     = "MLE"
    RE      = "RE"
    CE      = "CE"


class CheckerType(str, enum.Enum):
    NCMP   = "ncmp"
    WCMP   = "wcmp"
    RCMP4  = "rcmp4"
    RCMP6  = "rcmp6"
    CUSTOM = "custom"


class ScoringType(str, enum.Enum):
    ICPC       = "icpc"
    IOI_SUM    = "ioi_sum"
    IOI_GROUPS = "ioi_groups"
    MOSH       = "mosh"       # частичные баллы через quitp в чекере


class ContestFormat(str, enum.Enum):
    POLYGON = "polygon"
    MOSH    = "mosh"


class Topic(Base):
    __tablename__ = "topics"

    id:    Mapped[int] = mapped_column(Integer, primary_key=True)
    slug:  Mapped[str] = mapped_column(String(64), unique=True, index=True)
    title: Mapped[str] = mapped_column(String(128))

    tasks: Mapped[list["Task"]] = relationship(back_populates="topic")


class Task(Base):
    __tablename__ = "tasks"

    id:             Mapped[int]          = mapped_column(Integer, primary_key=True)
    topic_id:       Mapped[int | None]   = mapped_column(ForeignKey("topics.id"), nullable=True)
    order_in_topic: Mapped[int]          = mapped_column(Integer, default=1)
    title:          Mapped[str]          = mapped_column(String(256))
    statement_md:   Mapped[str | None]   = mapped_column(Text, nullable=True)
    statement_html: Mapped[str | None]   = mapped_column(Text, nullable=True)
    time_limit:     Mapped[float]        = mapped_column(Float, default=1.0)
    memory_limit:   Mapped[int]          = mapped_column(Integer, default=256)
    tests_path:     Mapped[str]          = mapped_column(String(512))
    checker_path:   Mapped[str]          = mapped_column(String(512))
    scoring_type:     Mapped[ScoringType]  = mapped_column(SAEnum(ScoringType), default=ScoringType.ICPC)
    max_score:        Mapped[float]        = mapped_column(Float, default=0.0)
    test_scores_json: Mapped[str | None]   = mapped_column(Text, nullable=True)  # JSON список баллов за каждый тест
    is_output_only:   Mapped[bool]         = mapped_column(Boolean, default=False)  # MOSH: участник сдаёт готовый файл-ответ, а не код
    is_interactive:    Mapped[bool]        = mapped_column(Boolean, default=False)  # Polygon: есть interactor, программа общается с ним в реальном времени
    interactor_path:   Mapped[str | None]  = mapped_column(String(512), nullable=True)  # путь к скомпилированному interactor (для отладки, судья пока не запускает)

    topic:         Mapped["Topic | None"]      = relationship(back_populates="tasks")
    submissions:   Mapped[list["Submission"]]  = relationship(back_populates="task")
    subtasks:      Mapped[list["Subtask"]]     = relationship(back_populates="task")
    contest_tasks: Mapped[list["ContestTask"]] = relationship(back_populates="task")


class Subtask(Base):
    __tablename__ = "subtasks"

    id:               Mapped[int]        = mapped_column(Integer, primary_key=True)
    task_id:          Mapped[int]        = mapped_column(ForeignKey("tasks.id"))
    number:           Mapped[int]        = mapped_column(Integer)
    max_score:        Mapped[float]      = mapped_column(Float)
    test_from:        Mapped[int]        = mapped_column(Integer)
    test_to:          Mapped[int]        = mapped_column(Integer)
    depends_on_json:  Mapped[str | None] = mapped_column(Text, nullable=True)  # JSON-список номеров Subtask, которые должны быть пройдены первыми

    task: Mapped["Task"] = relationship(back_populates="subtasks")


class Contest(Base):
    __tablename__ = "contests"

    id:          Mapped[int]        = mapped_column(Integer, primary_key=True)
    title:       Mapped[str]        = mapped_column(String(256))
    year:        Mapped[int | None] = mapped_column(Integer, nullable=True)
    description: Mapped[str | None] = mapped_column(Text, nullable=True)
    format:      Mapped[ContestFormat] = mapped_column(SAEnum(ContestFormat), default=ContestFormat.POLYGON)
    created_at:  Mapped[datetime]   = mapped_column(DateTime, default=datetime.now)

    contest_tasks: Mapped[list["ContestTask"]] = relationship(back_populates="contest")


class ContestTask(Base):
    __tablename__ = "contest_tasks"

    id:         Mapped[int] = mapped_column(Integer, primary_key=True)
    contest_id: Mapped[int] = mapped_column(ForeignKey("contests.id"))
    task_id:    Mapped[int] = mapped_column(ForeignKey("tasks.id"))
    letter:     Mapped[str] = mapped_column(String(8))
    order:      Mapped[int] = mapped_column(Integer, default=1)

    contest: Mapped["Contest"] = relationship(back_populates="contest_tasks")
    task:    Mapped["Task"]    = relationship(back_populates="contest_tasks")


class User(Base):
    __tablename__ = "users"

    id:            Mapped[int]      = mapped_column(Integer, primary_key=True)
    username:      Mapped[str]      = mapped_column(String(64), unique=True, index=True)
    email:         Mapped[str]      = mapped_column(String(256), unique=True)
    password_hash: Mapped[str]      = mapped_column(String(256))
    is_admin:      Mapped[bool]     = mapped_column(Boolean, default=False)
    created_at:    Mapped[datetime] = mapped_column(DateTime, default=datetime.now)

    submissions: Mapped[list["Submission"]] = relationship(back_populates="user")


class Submission(Base):
    __tablename__ = "submissions"

    id:                     Mapped[int]          = mapped_column(Integer, primary_key=True)
    user_id:                Mapped[int]          = mapped_column(ForeignKey("users.id"))
    task_id:                Mapped[int]          = mapped_column(ForeignKey("tasks.id"))
    task_submission_number: Mapped[int]          = mapped_column(Integer, default=1)
    code:                   Mapped[str]          = mapped_column(Text)
    language:               Mapped[str]          = mapped_column(String(16), default="cpp")
    verdict:                Mapped[Verdict]      = mapped_column(SAEnum(Verdict), default=Verdict.PENDING)
    failed_test:            Mapped[int | None]   = mapped_column(Integer, nullable=True)
    error_output:           Mapped[str | None]   = mapped_column(Text, nullable=True)
    execution_time:         Mapped[float | None] = mapped_column(Float, nullable=True)
    score:                  Mapped[float | None] = mapped_column(Float, nullable=True)
    ai_hint:                Mapped[str | None]   = mapped_column(Text, nullable=True)
    submitted_at:           Mapped[datetime]     = mapped_column(DateTime, default=datetime.now)

    user:         Mapped["User"]             = relationship(back_populates="submissions")
    task:         Mapped["Task"]             = relationship(back_populates="submissions")
    test_results: Mapped[list["TestResult"]] = relationship(back_populates="submission")


class TestResult(Base):
    __tablename__ = "test_results"

    id:             Mapped[int]          = mapped_column(Integer, primary_key=True)
    submission_id:  Mapped[int]          = mapped_column(ForeignKey("submissions.id"))
    test_number:    Mapped[int]          = mapped_column(Integer)
    verdict:        Mapped[Verdict]      = mapped_column(SAEnum(Verdict))
    execution_time: Mapped[float | None] = mapped_column(Float, nullable=True)
    score:          Mapped[float]        = mapped_column(Float, default=0.0)

    submission: Mapped["Submission"] = relationship(back_populates="test_results")