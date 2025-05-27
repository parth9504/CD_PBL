from prettytable import PrettyTable
import re

def clean_code(code_lines):
    """Remove empty lines and strip whitespace."""
    return [line.strip() for line in code_lines if line.strip()]

def transform_for_to_while(code_lines):
    """Transform for loops into while loops."""
    transformed = []
    i = 0
    while i < len(code_lines):
        line = code_lines[i]
        # Match for loop: e.g., for(i=0; i<10; i++)
        if re.match(r'for\s*\(.*;.*;.*\)', line):
            try:
                # Extract init, cond, inc from for(init;cond;inc)
                content = line[line.index('(')+1:line.index(')')].strip()
                parts = content.split(';')
                if len(parts) != 3:
                    raise ValueError(f"Invalid for loop syntax: {line}")
                init, cond, inc = [part.strip() for part in parts]
                if not init or not cond or not inc:
                    raise ValueError(f"Missing components in for loop: {line}")
                
                transformed.append(init + ';')  # e.g., i=0;
                transformed.append(f'while({cond})' + ' {')  # e.g., while(i<10) {
                i += 1
                brace_count = 1
                while i < len(code_lines) and brace_count > 0:
                    curr_line = code_lines[i].strip()
                    if curr_line == '{':
                        brace_count += 1
                    elif curr_line == '}':
                        brace_count -= 1
                    if brace_count > 0:
                        transformed.append(curr_line)
                    i += 1
                transformed.append(inc + ';')  # e.g., i++;
                transformed.append('}')
            except Exception as e:
                print(f"Error parsing for loop at line {i+1}: {e}")
                transformed.append(line)  # Fallback: keep original line
        else:
            transformed.append(line)
            i += 1
    return transformed

def generate_TAC(code_lines):
    """Generate three-address code from transformed code."""
    tac = []
    jump_stack = []
    label_counter = 1
    i = 0

    while i < len(code_lines):
        line = code_lines[i].strip()
        if line.startswith('while'):
            try:
                cond = line[line.index('(')+1:line.index(')')].strip()
                start_label = f'L{label_counter}'
                label_counter += 1
                tac.append(f'{start_label}:')  # Label for loop start
                exit_label = f'L{label_counter}'
                tac.append(f'if !({cond}) goto {exit_label}')
                jump_stack.append((start_label, exit_label))
                i += 1
            except Exception as e:
                print(f"Error parsing while loop at line {i+1}: {e}")
                tac.append(line)
                i += 1
        elif line == '}':
            if not jump_stack:
                print(f"Error: Mismatched closing brace at line {i+1}")
                i += 1
                continue
            start_label, exit_label = jump_stack.pop()
            tac.append(f'goto {start_label}')
            tac.append(f'{exit_label}:')
            i += 1
        elif line.startswith('if'):
            try:
                cond = line[line.index('(')+1:line.index(')')].strip()
                false_label = f'L{label_counter}'
                label_counter += 1
                tac.append(f'if !({cond}) goto {false_label}')
                jump_stack.append(('if', false_label))
                i += 1
            except Exception as e:
                print(f"Error parsing if statement at line {i+1}: {e}")
                tac.append(line)
                i += 1
        elif line.startswith('else'):
            if not jump_stack or jump_stack[-1][0] != 'if':
                print(f"Error: else without matching if at line {i+1}")
                i += 1
                continue
            end_label = f'L{label_counter}'
            label_counter += 1
            tac.append(f'goto {end_label}')
            _, false_label = jump_stack.pop()
            tac.append(f'{false_label}:')
            jump_stack.append(('else', end_label))
            i += 1
        elif line == 'endif':
            if not jump_stack or jump_stack[-1][0] not in ('if', 'else'):
                print(f"Error: endif without matching if/else at line {i+1}")
                i += 1
                continue
            _, label = jump_stack.pop()
            tac.append(f'{label}:')
            i += 1
        else:
            tac.append(line)
            i += 1

    tac.append('END')
    return tac

def display_TAC(tac_code):
    """Display TAC in a PrettyTable."""
    table = PrettyTable()
    table.field_names = ["Index", "Code"]
    for i, line in enumerate(tac_code, start=1):
        table.add_row([i, line])
    print("\nThe Three Address Code Generated is:\n")
    print(table)

# === Main Driver ===
if __name__ == "__main__":
    try:
        with open("code3.txt", "r") as f:
            code_lines = f.readlines()
    except FileNotFoundError:
        print("Error: code.txt not found")
        exit(1)
    except Exception as e:
        print(f"Error reading code.txt: {e}")
        exit(1)

    print("Input Code:\n")
    print(''.join(code_lines))

    cleaned = clean_code(code_lines)
    transformed = transform_for_to_while(cleaned)
    tac_code = generate_TAC(transformed)
    display_TAC(tac_code)