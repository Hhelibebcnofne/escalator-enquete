export type User = {
  id: number;
  name: string;
};

export type Users = User[];

export type Question = {
  id: number;
  sentence: string;
  optionA: string;
  optionB: string;
};

export type Questions = Question[];
