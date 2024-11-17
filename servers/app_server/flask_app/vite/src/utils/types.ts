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

export type CountWithNames = {
  sentence: string;
  optionA: string;
  optionB: string;
  optionA_count: number;
  optionB_count: number;
  error_count: number;
};

export type Statistics = {
  [id: number]: CountWithNames;
};
